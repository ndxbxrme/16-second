#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cmath>

SixteenSecondAudioProcessor::SixteenSecondAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMS", createParameterLayout())
{
}

SixteenSecondAudioProcessor::~SixteenSecondAudioProcessor() = default;

const juce::String SixteenSecondAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SixteenSecondAudioProcessor::acceptsMidi() const
{
    return true;
}

bool SixteenSecondAudioProcessor::producesMidi() const
{
    return false;
}

bool SixteenSecondAudioProcessor::isMidiEffect() const
{
    return false;
}

double SixteenSecondAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SixteenSecondAudioProcessor::getNumPrograms()
{
    return 1;
}

int SixteenSecondAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SixteenSecondAudioProcessor::setCurrentProgram(int)
{
}

const juce::String SixteenSecondAudioProcessor::getProgramName(int)
{
    return {};
}

void SixteenSecondAudioProcessor::changeProgramName(int, const juce::String&)
{
}

void SixteenSecondAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    const auto maxSeconds = 16.0;
    maxBufferSamples = static_cast<int>(std::ceil(sampleRate * maxSeconds));
    memoryBuffer.prepare(getTotalNumInputChannels(), maxBufferSamples);
    tempFloatBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);
    delaySmoother.reset(sampleRate, 0.0f, 10.0f);
    feedbackModel.reset(sampleRate);
    limiterL.reset(sampleRate);
    limiterR.reset(sampleRate);
    resetLoopState();
}

void SixteenSecondAudioProcessor::releaseResources()
{
}

bool SixteenSecondAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainOut = layouts.getMainOutputChannelSet();
    if (mainOut != juce::AudioChannelSet::mono() && mainOut != juce::AudioChannelSet::stereo())
        return false;

    const auto& mainIn = layouts.getMainInputChannelSet();
    return mainIn == mainOut;
}

template <typename SampleType>
void SixteenSecondAudioProcessor::processBlockInternal(juce::AudioBuffer<SampleType>& buffer)
{
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    const auto delayMs = apvts.getRawParameterValue("delayTime")->load();
    const auto feedback = apvts.getRawParameterValue("feedback")->load();
    const auto mix = apvts.getRawParameterValue("mix")->load();
    const auto overdubLevel = apvts.getRawParameterValue("overdubLevel")->load();
    const auto erodeAmount = apvts.getRawParameterValue("erodeAmount")->load();
    const auto filterAmount = apvts.getRawParameterValue("filter")->load();
    const auto noiseAmount = apvts.getRawParameterValue("noise")->load();
    const auto gainDb = apvts.getRawParameterValue("outputGain")->load();
    const auto limiterOn = apvts.getRawParameterValue("limiter")->load() > 0.5f;
    const auto gain = juce::Decibels::decibelsToGain(static_cast<SampleType>(gainDb));
    const auto isRecording = apvts.getRawParameterValue("record")->load() > 0.5f;
    const auto isPlaying = apvts.getRawParameterValue("play")->load() > 0.5f;
    const auto isOverdubbing = apvts.getRawParameterValue("overdub")->load() > 0.5f;
    const auto isClear = apvts.getRawParameterValue("clear")->load() > 0.5f;
    const auto isHalfSpeed = apvts.getRawParameterValue("halfSpeed")->load() > 0.5f;
    const auto isReverse = apvts.getRawParameterValue("reverse")->load() > 0.5f;
    const auto isAuthentic = apvts.getRawParameterValue("authentic")->load() > 0.5f;

    if (maxBufferSamples <= 0 || memoryBuffer.getSize() <= 0)
        return;

    const auto targetDelaySamples =
        juce::jlimit(0, maxBufferSamples - 1,
                     static_cast<int>(delayMs * (getSampleRate() / 1000.0)));

    if (!isAuthentic)
    {
        delaySmoother.setTimeMs(10.0f);
        delaySmoother.setTarget(static_cast<float>(targetDelaySamples));
    }
    else
    {
        delaySmoother.setTarget(static_cast<float>(targetDelaySamples));
        delaySmoother.process();
    }

    const auto mixClamped = juce::jlimit(0.0f, 1.0f, mix);
    const auto dryGain = std::cos(mixClamped * juce::MathConstants<float>::halfPi);
    const auto wetGain = std::sin(mixClamped * juce::MathConstants<float>::halfPi);

    limiterL.setThreshold(0.98f);
    limiterR.setThreshold(0.98f);

    const auto clearEdge = isClear && !lastClear;
    lastClear = isClear;

    const auto hasLoop = loopLengthSamples > 0;
    const auto nextState = stateMachine.update(isRecording, isPlaying, isOverdubbing, hasLoop, clearEdge);

    if (clearEdge)
        resetLoopState();

    if (currentState != nextState)
    {
        if (currentState == LoopState::Record && nextState != LoopState::Record)
        {
            loopLengthSamples = juce::jlimit(1, maxBufferSamples, recordedSamples);
            loopStartIndex = memoryBuffer.getWriteIndex() - loopLengthSamples;
            if (loopStartIndex < 0)
                loopStartIndex += maxBufferSamples;
            loopReadIndex = loopStartIndex;
            loopStepper.reset(0.0);
        }

        if ((nextState == LoopState::Play || nextState == LoopState::Overdub) &&
            (currentState != LoopState::Play && currentState != LoopState::Overdub))
        {
            loopReadIndex = loopStartIndex;
            loopStepper.reset(0.0);
        }
    }

    currentState = nextState;

    if (currentState == LoopState::Record)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            for (int channel = 0; channel < numChannels; ++channel)
            {
                const auto input = buffer.getSample(channel, i);
                const auto degraded = feedbackModel.process(static_cast<float>(input),
                                                            filterAmount,
                                                            noiseAmount,
                                                            1.0f,
                                                            generateNoise());
                memoryBuffer.writeSample(channel, memoryBuffer.getWriteIndex(), degraded);
                auto output = static_cast<float>(input * gain);
                if (limiterOn)
                    output = (channel == 0) ? limiterL.process(output) : limiterR.process(output);
                buffer.setSample(channel, i, static_cast<SampleType>(output));
            }

            memoryBuffer.advanceWrite();
            if (recordedSamples < maxBufferSamples)
                ++recordedSamples;
        }

        return;
    }

    if ((currentState == LoopState::Play || currentState == LoopState::Overdub) && loopLengthSamples > 0)
    {
        const auto rateSign = isReverse ? -1.0 : 1.0;
        const auto rate = (isHalfSpeed ? 0.5 : 1.0) * rateSign;
        loopStepper.setRate(rate);

        for (int i = 0; i < numSamples; ++i)
        {
            const auto readIndex = loopStartIndex + loopStepper.getIndex(loopLengthSamples);

            for (int channel = 0; channel < numChannels; ++channel)
            {
                const auto input = buffer.getSample(channel, i);
                const auto readSample = memoryBuffer.readSample(channel, readIndex);
                const auto mixed = static_cast<SampleType>(input * dryGain + readSample * wetGain);
                auto output = static_cast<float>(mixed * gain);
                if (limiterOn)
                    output = (channel == 0) ? limiterL.process(output) : limiterR.process(output);
                buffer.setSample(channel, i, static_cast<SampleType>(output));

                if (currentState == LoopState::Overdub)
                {
                    const auto existing = readSample;
                    const auto overdubWrite = Overdub::apply(existing,
                                                             static_cast<float>(input),
                                                             readSample,
                                                             overdubLevel,
                                                             feedback,
                                                             erodeAmount);
                    const auto degraded = feedbackModel.process(overdubWrite,
                                                                filterAmount,
                                                                noiseAmount,
                                                                1.0f,
                                                                generateNoise());
                    memoryBuffer.writeSample(channel, readIndex, degraded);
                }
            }

            loopStepper.advance();
        }
        return;
    }

    for (int i = 0; i < numSamples; ++i)
    {
        const auto delaySamples = isAuthentic
                                      ? static_cast<float>(targetDelaySamples)
                                      : delaySmoother.process();
        const auto writeIndex = memoryBuffer.getWriteIndex();
        const auto readIndex = static_cast<float>(writeIndex) - delaySamples;

        for (int channel = 0; channel < numChannels; ++channel)
        {
            const auto input = buffer.getSample(channel, i);
            const auto readSample = isAuthentic ? memoryBuffer.readSample(channel, static_cast<int>(readIndex))
                                                 : memoryBuffer.readSampleLinear(channel, readIndex);
            const auto feedbackSignal = feedbackModel.process(readSample,
                                                              filterAmount,
                                                              noiseAmount,
                                                              feedback,
                                                              generateNoise());
            const auto writeValue = static_cast<float>(input + feedbackSignal);
            memoryBuffer.writeSample(channel, writeIndex, writeValue);
            const auto mixed = static_cast<SampleType>(input * dryGain + readSample * wetGain);
            auto output = static_cast<float>(mixed * gain);
            if (limiterOn)
                output = (channel == 0) ? limiterL.process(output) : limiterR.process(output);
            buffer.setSample(channel, i, static_cast<SampleType>(output));
        }

        memoryBuffer.advanceWrite();
    }
}

void SixteenSecondAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    processBlockInternal(buffer);
    updateMeters(buffer);
}

void SixteenSecondAudioProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    tempFloatBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples(), false, false, true);
    tempFloatBuffer.clear();

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* src = buffer.getReadPointer(channel);
        auto* dst = tempFloatBuffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            dst[i] = static_cast<float>(src[i]);
    }

    processBlockInternal(tempFloatBuffer);

    updateMeters(tempFloatBuffer);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* src = tempFloatBuffer.getReadPointer(channel);
        auto* dst = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            dst[i] = static_cast<double>(src[i]);
    }
}

void SixteenSecondAudioProcessor::updateMeters(const juce::AudioBuffer<float>& buffer)
{
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();
    const auto holdSamples = static_cast<int>(getSampleRate() * 0.5);

    float peakL = 0.0f;
    float peakR = 0.0f;

    if (numChannels > 0)
    {
        const auto* dataL = buffer.getReadPointer(0);
        for (int i = 0; i < numSamples; ++i)
            peakL = std::max(peakL, std::abs(dataL[i]));
    }

    if (numChannels > 1)
    {
        const auto* dataR = buffer.getReadPointer(1);
        for (int i = 0; i < numSamples; ++i)
            peakR = std::max(peakR, std::abs(dataR[i]));
    }

    const float decay = 0.90f;
    const auto newL = std::max(peakL, meterL.load() * decay);
    const auto newR = std::max(peakR, meterR.load() * decay);

    meterL.store(newL);
    meterR.store(newR);

    if (peakL > 1.0f || peakR > 1.0f)
    {
        clipHold.store(holdSamples);
        clipFlag.store(true);
    }
    else
    {
        auto remaining = clipHold.load();
        remaining = std::max(0, remaining - numSamples);
        clipHold.store(remaining);
        if (remaining == 0)
            clipFlag.store(false);
    }
}

bool SixteenSecondAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SixteenSecondAudioProcessor::createEditor()
{
    return new SixteenSecondAudioProcessorEditor(*this);
}

void SixteenSecondAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState().createXml())
        copyXmlToBinary(*state, destData);
}

void SixteenSecondAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xmlState = getXmlFromBinary(data, sizeInBytes))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout SixteenSecondAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "delayTime",
        "Delay Time",
        juce::NormalisableRange<float>{0.0f, 16000.0f, 1.0f},
        450.0f,
        "ms"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "feedback",
        "Feedback",
        juce::NormalisableRange<float>{0.0f, 1.2f, 0.001f},
        0.65f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "mix",
        "Mix",
        juce::NormalisableRange<float>{0.0f, 1.0f, 0.001f},
        0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "overdubLevel",
        "Overdub Level",
        juce::NormalisableRange<float>{0.0f, 1.0f, 0.001f},
        0.6f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "erodeAmount",
        "Erode Amount",
        juce::NormalisableRange<float>{0.0f, 1.0f, 0.001f},
        0.35f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "outputGain",
        "Output Gain",
        juce::NormalisableRange<float>{-24.0f, 12.0f, 0.01f},
        0.0f,
        "dB"));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "record",
        "Record",
        false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "play",
        "Play",
        false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "overdub",
        "Overdub",
        false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "clear",
        "Clear",
        false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "halfSpeed",
        "Half-speed",
        false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "reverse",
        "Reverse",
        false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "authentic",
        "Authentic",
        false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filter",
        "Filter",
        juce::NormalisableRange<float>{0.0f, 1.0f, 0.001f},
        0.6f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "noise",
        "Noise/Grit",
        juce::NormalisableRange<float>{0.0f, 1.0f, 0.001f},
        0.25f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "limiter",
        "Limiter",
        true));

    return {params.begin(), params.end()};
}

void SixteenSecondAudioProcessor::resetLoopState()
{
    memoryBuffer.clear();
    loopLengthSamples = 0;
    loopStartIndex = 0;
    loopReadIndex = 0;
    recordedSamples = 0;
    loopStepper.reset(0.0);
    delaySmoother.reset(getSampleRate(), 0.0f, 10.0f);
    feedbackModel.reset(getSampleRate());
    limiterL.reset(getSampleRate());
    limiterR.reset(getSampleRate());
    currentState = LoopState::Idle;
    lastClear = false;
    noiseSeed = 0x1234567u;
}

float SixteenSecondAudioProcessor::generateNoise()
{
    noiseSeed = noiseSeed * 1664525u + 1013904223u;
    const auto value = static_cast<float>((noiseSeed >> 8) & 0x00FFFFFFu) / 16777215.0f;
    return value;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SixteenSecondAudioProcessor();
}
