#pragma once

#include <JuceHeader.h>

#include "dsp/MemoryBuffer.h"
#include "dsp/Overdub.h"
#include "dsp/RateStepper.h"
#include "dsp/StateMachine.h"

class SixteenSecondAudioProcessor final : public juce::AudioProcessor
{
public:
    SixteenSecondAudioProcessor();
    ~SixteenSecondAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    juce::AudioProcessorValueTreeState apvts;

    template <typename SampleType>
    void processBlockInternal(juce::AudioBuffer<SampleType>& buffer);

    void resetLoopState();

    MemoryBuffer memoryBuffer;
    StateMachine stateMachine;
    RateStepper loopStepper;
    juce::AudioBuffer<float> tempFloatBuffer;

    int maxBufferSamples = 0;
    int loopLengthSamples = 0;
    int loopStartIndex = 0;
    int loopReadIndex = 0;
    int recordedSamples = 0;
    LoopState currentState = LoopState::Idle;
    bool lastClear = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SixteenSecondAudioProcessor)
};
