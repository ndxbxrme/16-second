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

    filterSlider.setSliderStyle(juce::Slider::LinearVertical);
    filterSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    filterSlider.setRange(0.0, 1.0, 0.001);
    filterLabel.setText("Filter", juce::dontSendNotification);
    filterLabel.setJustificationType(juce::Justification::centred);
    filterLabel.attachToComponent(&filterSlider, false);
    addAndMakeVisible(filterSlider);
    addAndMakeVisible(filterLabel);
    filterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "filter", filterSlider);

    noiseSlider.setSliderStyle(juce::Slider::LinearVertical);
    noiseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    noiseSlider.setRange(0.0, 1.0, 0.001);
    noiseLabel.setText("Noise", juce::dontSendNotification);
    noiseLabel.setJustificationType(juce::Justification::centred);
    noiseLabel.attachToComponent(&noiseSlider, false);
    addAndMakeVisible(noiseSlider);
    addAndMakeVisible(noiseLabel);
    noiseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "noise", noiseSlider);

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

    halfSpeedButton.setButtonText("Half");
    reverseButton.setButtonText("Reverse");
    addAndMakeVisible(halfSpeedButton);
    addAndMakeVisible(reverseButton);

    halfSpeedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "halfSpeed", halfSpeedButton);
    reverseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "reverse", reverseButton);

    authenticButton.setButtonText("Authentic");
    addAndMakeVisible(authenticButton);

    authenticAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "authentic", authenticButton);

    limiterButton.setButtonText("Limiter");
    addAndMakeVisible(limiterButton);

    limiterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "limiter", limiterButton);

    setSize(1280, 280);
    startTimerHz(30);
}

SixteenSecondAudioProcessorEditor::~SixteenSecondAudioProcessorEditor() = default;

void SixteenSecondAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawFittedText("16-Second (Milestone 6+Limiter)", getLocalBounds().removeFromTop(30),
                    juce::Justification::centred, 1);

    g.setColour(juce::Colours::darkgrey);
    g.fillRect(meterArea);

    const auto meterHeight = meterArea.getHeight();
    const auto meterWidth = meterArea.getWidth() / 2 - 4;

    const auto filledL = static_cast<int>(meterHeight * juce::jlimit(0.0f, 1.0f, meterL));
    const auto filledR = static_cast<int>(meterHeight * juce::jlimit(0.0f, 1.0f, meterR));

    const auto leftMeter = juce::Rectangle<int>(meterArea.getX() + 4,
                                                meterArea.getBottom() - filledL,
                                                meterWidth,
                                                filledL);
    const auto rightMeter = juce::Rectangle<int>(meterArea.getX() + meterWidth + 8,
                                                 meterArea.getBottom() - filledR,
                                                 meterWidth,
                                                 filledR);

    g.setColour(juce::Colours::limegreen);
    g.fillRect(leftMeter);
    g.fillRect(rightMeter);

    g.setColour(clipOn ? juce::Colours::red : juce::Colours::darkred);
    g.fillEllipse(clipLedArea.toFloat());
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
    filterSlider.setBounds(sliderArea.removeFromLeft(sliderWidth));
    noiseSlider.setBounds(sliderArea.removeFromLeft(sliderWidth));
    outputGainSlider.setBounds(sliderArea.removeFromLeft(sliderWidth));

    auto buttonArea = area.removeFromTop(30);
    recordButton.setBounds(buttonArea.removeFromLeft(120));
    playButton.setBounds(buttonArea.removeFromLeft(120));
    overdubButton.setBounds(buttonArea.removeFromLeft(120));
    clearButton.setBounds(buttonArea.removeFromLeft(120));
    halfSpeedButton.setBounds(buttonArea.removeFromLeft(120));
    reverseButton.setBounds(buttonArea.removeFromLeft(120));
    authenticButton.setBounds(buttonArea.removeFromLeft(140));
    limiterButton.setBounds(buttonArea.removeFromLeft(120));

    meterArea = area.removeFromRight(80).withTrimmedTop(10).withTrimmedBottom(10);
    const auto led = meterArea.removeFromTop(16);
    clipLedArea = led.withSizeKeepingCentre(12, 12);
}

void SixteenSecondAudioProcessorEditor::timerCallback()
{
    meterL = processor.getMeterL();
    meterR = processor.getMeterR();
    clipOn = processor.getClip();
    repaint();
}
