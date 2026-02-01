#include "PluginEditor.h"

SixteenSecondAudioProcessorEditor::SixteenSecondAudioProcessorEditor(SixteenSecondAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    delayTimeSlider.setSliderStyle(juce::Slider::LinearVertical);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    delayTimeSlider.setRange(0.0, 16000.0, 1.0);
    delayTimeLabel.setText("Delay", juce::dontSendNotification);
    delayTimeLabel.setJustificationType(juce::Justification::centred);
    delayTimeLabel.attachToComponent(&delayTimeSlider, false);
    addAndMakeVisible(delayTimeSlider);
    addAndMakeVisible(delayTimeLabel);
    delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "delayTime", delayTimeSlider);

    feedbackSlider.setSliderStyle(juce::Slider::LinearVertical);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    feedbackSlider.setRange(0.0, 1.2, 0.001);
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    feedbackLabel.attachToComponent(&feedbackSlider, false);
    addAndMakeVisible(feedbackSlider);
    addAndMakeVisible(feedbackLabel);
    feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "feedback", feedbackSlider);

    mixSlider.setSliderStyle(juce::Slider::LinearVertical);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    mixSlider.setRange(0.0, 1.0, 0.001);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.attachToComponent(&mixSlider, false);
    addAndMakeVisible(mixSlider);
    addAndMakeVisible(mixLabel);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "mix", mixSlider);

    overdubLevelSlider.setSliderStyle(juce::Slider::LinearVertical);
    overdubLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    overdubLevelSlider.setRange(0.0, 1.0, 0.001);
    overdubLevelLabel.setText("Overdub", juce::dontSendNotification);
    overdubLevelLabel.setJustificationType(juce::Justification::centred);
    overdubLevelLabel.attachToComponent(&overdubLevelSlider, false);
    addAndMakeVisible(overdubLevelSlider);
    addAndMakeVisible(overdubLevelLabel);
    overdubLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "overdubLevel", overdubLevelSlider);

    erodeAmountSlider.setSliderStyle(juce::Slider::LinearVertical);
    erodeAmountSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    erodeAmountSlider.setRange(0.0, 1.0, 0.001);
    erodeAmountLabel.setText("Erode", juce::dontSendNotification);
    erodeAmountLabel.setJustificationType(juce::Justification::centred);
    erodeAmountLabel.attachToComponent(&erodeAmountSlider, false);
    addAndMakeVisible(erodeAmountSlider);
    addAndMakeVisible(erodeAmountLabel);
    erodeAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "erodeAmount", erodeAmountSlider);

    outputGainSlider.setSliderStyle(juce::Slider::LinearVertical);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    outputGainSlider.setRange(-24.0, 12.0, 0.01);

    outputGainLabel.setText("Output", juce::dontSendNotification);
    outputGainLabel.setJustificationType(juce::Justification::centred);
    outputGainLabel.attachToComponent(&outputGainSlider, false);

    addAndMakeVisible(outputGainSlider);
    addAndMakeVisible(outputGainLabel);

    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "outputGain", outputGainSlider);

    recordButton.setButtonText("Record");
    playButton.setButtonText("Play");
    addAndMakeVisible(recordButton);
    addAndMakeVisible(playButton);

    recordAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "record", recordButton);
    playAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "play", playButton);

    overdubButton.setButtonText("Overdub");
    clearButton.setButtonText("Clear");
    clearButton.setClickingTogglesState(true);
    addAndMakeVisible(overdubButton);
    addAndMakeVisible(clearButton);

    overdubAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "overdub", overdubButton);
    clearAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "clear", clearButton);

    setSize(720, 280);
}

SixteenSecondAudioProcessorEditor::~SixteenSecondAudioProcessorEditor() = default;

void SixteenSecondAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawFittedText("16-Second (Milestone 3)", getLocalBounds().removeFromTop(30),
                    juce::Justification::centred, 1);
}

void SixteenSecondAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(40);

    auto sliderArea = area.removeFromTop(180);
    const auto sliderWidth = 100;

    delayTimeSlider.setBounds(sliderArea.removeFromLeft(sliderWidth));
    feedbackSlider.setBounds(sliderArea.removeFromLeft(sliderWidth));
    mixSlider.setBounds(sliderArea.removeFromLeft(sliderWidth));
    overdubLevelSlider.setBounds(sliderArea.removeFromLeft(sliderWidth));
    erodeAmountSlider.setBounds(sliderArea.removeFromLeft(sliderWidth));
    outputGainSlider.setBounds(sliderArea.removeFromLeft(sliderWidth));

    auto buttonArea = area.removeFromTop(30);
    recordButton.setBounds(buttonArea.removeFromLeft(120));
    playButton.setBounds(buttonArea.removeFromLeft(120));
    overdubButton.setBounds(buttonArea.removeFromLeft(120));
    clearButton.setBounds(buttonArea.removeFromLeft(120));
}
