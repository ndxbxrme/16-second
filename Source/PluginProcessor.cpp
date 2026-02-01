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
    const auto gainDb = apvts.getRawParameterValue("outputGain")->load();
    const auto gain = juce::Decibels::decibelsToGain(static_cast<SampleType>(gainDb));
    const auto isRecording = apvts.getRawParameterValue("record")->load() > 0.5f;
    const auto isPlaying = apvts.getRawParameterValue("play")->load() > 0.5f;
    const auto isOverdubbing = apvts.getRawParameterValue("overdub")->load() > 0.5f;
    const auto isClear = apvts.getRawParameterValue("clear")->load() > 0.5f;
    const auto isHalfSpeed = apvts.getRawParameterValue("halfSpeed")->load() > 0.5f;
    const auto isReverse = apvts.getRawParameterValue("reverse")->load() > 0.5f;

    if (maxBufferSamples <= 0 || memoryBuffer.getSize() <= 0)
        return;

    const auto delaySamples = juce::jlimit(0, maxBufferSamples - 1,
                                           static_cast<int>(delayMs * (getSampleRate() / 1000.0)));

    const auto mixClamped = juce::jlimit(0.0f, 1.0f, mix);
    const auto dryGain = std::cos(mixClamped * juce::MathConstants<float>::halfPi);
    const auto wetGain = std::sin(mixClamped * juce::MathConstants<float>::halfPi);

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
                memoryBuffer.writeSample(channel, memoryBuffer.getWriteIndex(), static_cast<float>(input));
                buffer.setSample(channel, i, static_cast<SampleType>(input * gain));
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
                buffer.setSample(channel, i, mixed * gain);

                if (currentState == LoopState::Overdub)
                {
                    const auto existing = readSample;
                    const auto writeValue = Overdub::apply(existing,
                                                           static_cast<float>(input),
                                                           readSample,
                                                           overdubLevel,
                                                           feedback,
                                                           erodeAmount);
                    memoryBuffer.writeSample(channel, readIndex, writeValue);
                }
            }

            loopStepper.advance();
        }
        return;
    }

    for (int i = 0; i < numSamples; ++i)
    {
        const auto writeIndex = memoryBuffer.getWriteIndex();
        const auto readIndex = writeIndex - delaySamples;

        for (int channel = 0; channel < numChannels; ++channel)
        {
            const auto input = buffer.getSample(channel, i);
            const auto readSample = memoryBuffer.readSample(channel, readIndex);
            const auto writeValue = static_cast<float>(input + readSample * feedback);
            memoryBuffer.writeSample(channel, writeIndex, writeValue);
            const auto mixed = static_cast<SampleType>(input * dryGain + readSample * wetGain);
            buffer.setSample(channel, i, mixed * gain);
        }

        memoryBuffer.advanceWrite();
    }
}

void SixteenSecondAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    processBlockInternal(buffer);
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

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* src = tempFloatBuffer.getReadPointer(channel);
        auto* dst = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            dst[i] = static_cast<double>(src[i]);
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
    currentState = LoopState::Idle;
    lastClear = false;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SixteenSecondAudioProcessor();
}
