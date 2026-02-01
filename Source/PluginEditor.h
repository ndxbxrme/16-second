#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class SixteenSecondAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                                private juce::Timer
{
public:
    explicit SixteenSecondAudioProcessorEditor(SixteenSecondAudioProcessor&);
    ~SixteenSecondAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    SixteenSecondAudioProcessor& processor;

    juce::Slider delayTimeSlider;
    juce::Label delayTimeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;

    juce::Slider feedbackSlider;
    juce::Label feedbackLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;

    juce::Slider mixSlider;
    juce::Label mixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    juce::Slider overdubLevelSlider;
    juce::Label overdubLevelLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> overdubLevelAttachment;

    juce::Slider erodeAmountSlider;
    juce::Label erodeAmountLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> erodeAmountAttachment;

    juce::Slider filterSlider;
    juce::Label filterLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterAttachment;

    juce::Slider noiseSlider;
    juce::Label noiseLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noiseAttachment;

    juce::Slider outputGainSlider;
    juce::Label outputGainLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;

    juce::ToggleButton recordButton;
    juce::ToggleButton playButton;
    juce::ToggleButton overdubButton;
    juce::TextButton clearButton;
    juce::ToggleButton halfSpeedButton;
    juce::ToggleButton reverseButton;
    juce::ToggleButton authenticButton;
    juce::ToggleButton limiterButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> recordAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> playAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> overdubAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> clearAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> halfSpeedAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> reverseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> authenticAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> limiterAttachment;

    juce::Rectangle<int> meterArea;
    juce::Rectangle<int> clipLedArea;
    float meterL = 0.0f;
    float meterR = 0.0f;
    bool clipOn = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SixteenSecondAudioProcessorEditor)
};
