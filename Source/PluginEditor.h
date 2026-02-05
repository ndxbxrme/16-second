#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct VisualStateTargets
{
    float postureCompression = 0.0f;
    float tiltRadians = 0.0f;
    float jitterAmount = 0.0f;
    float dangerAmount = 0.0f;
};

class SurfaceComponent final : public juce::Component
{
public:
    SurfaceComponent();

    void setTargets(const VisualStateTargets& newTargets);
    void setDebug(bool shouldExaggerate);
    void tick();

    juce::Component& getContent() { return content; }

    void paint(juce::Graphics& g) override;

    void setMeterData(float newMeterL, float newMeterR, bool newClip,
                      bool newRec, bool newPlay, bool newOverdub);

    void setVisualBounds(const juce::Rectangle<int>& newMeterArea,
                         const juce::Rectangle<int>& newClipLed,
                         const juce::Rectangle<int>& newRecLed,
                         const juce::Rectangle<int>& newPlayLed,
                         const juce::Rectangle<int>& newOverdubLed);

private:
    void updateTransform();

    juce::Component content;
    VisualStateTargets targets;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> posture;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> tilt;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> jitter;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> danger;
    float jitterPhase = 0.0f;
    bool debug = false;
    juce::AffineTransform currentTransform;

    juce::Rectangle<int> meterArea;
    juce::Rectangle<int> clipLedArea;
    juce::Rectangle<int> recLedArea;
    juce::Rectangle<int> playLedArea;
    juce::Rectangle<int> overdubLedArea;
    float meterL = 0.0f;
    float meterR = 0.0f;
    bool clipOn = false;
    bool recOn = false;
    bool playOn = false;
    bool overdubOn = false;
};

class SixteenSecondAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                                private juce::Timer
{
public:
    explicit SixteenSecondAudioProcessorEditor(SixteenSecondAudioProcessor&);
    ~SixteenSecondAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    SixteenSecondAudioProcessor& processor;
    SurfaceComponent surface;
    bool debugMorph = false;

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

    juce::Slider modDepthSlider;
    juce::Label modDepthLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modDepthAttachment;

    juce::Slider modSpeedSlider;
    juce::Label modSpeedLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modSpeedAttachment;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SixteenSecondAudioProcessorEditor)
};
