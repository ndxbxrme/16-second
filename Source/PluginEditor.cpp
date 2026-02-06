#include "PluginEditor.h"

namespace
{
    constexpr int kSliderWidth = 80;
    constexpr int kSliderHeight = 170;
    constexpr int kSliderGap = 6;
    constexpr int kSliderCount = 9;
    constexpr int kLeftColumnWidth = 210;
    constexpr int kRightPanelWidth = 120;
    constexpr int kMeterWidth = 34;
    constexpr int kMargin = 16;
    constexpr int kHeaderHeight = 56;
    constexpr bool kAnimateWaves = true;

    void configureSlider(juce::Slider& slider)
    {
        slider.setSliderStyle(juce::Slider::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    }
}

SixteenSecondAudioProcessorEditor::SixteenSecondAudioProcessorEditor(SixteenSecondAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(background);
    background.toBack();
    background.setInterceptsMouseClicks(false, false);
    background.setAnimationEnabled(kAnimateWaves);
    configureSlider(delayTimeSlider);
    delayTimeSlider.setRange(0.0, 16000.0, 1.0);
    delayTimeLabel.setText("Delay", juce::dontSendNotification);
    delayTimeLabel.setJustificationType(juce::Justification::centred);
    delayTimeLabel.attachToComponent(&delayTimeSlider, false);
    addAndMakeVisible(delayTimeSlider);
    addAndMakeVisible(delayTimeLabel);
    delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "delayTime", delayTimeSlider);

    configureSlider(feedbackSlider);
    feedbackSlider.setRange(0.0, 1.2, 0.001);
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    feedbackLabel.attachToComponent(&feedbackSlider, false);
    addAndMakeVisible(feedbackSlider);
    addAndMakeVisible(feedbackLabel);
    feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "feedback", feedbackSlider);

    configureSlider(mixSlider);
    mixSlider.setRange(0.0, 1.0, 0.001);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.attachToComponent(&mixSlider, false);
    addAndMakeVisible(mixSlider);
    addAndMakeVisible(mixLabel);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "mix", mixSlider);

    configureSlider(overdubLevelSlider);
    overdubLevelSlider.setRange(0.0, 1.0, 0.001);
    overdubLevelLabel.setText("Overdub", juce::dontSendNotification);
    overdubLevelLabel.setJustificationType(juce::Justification::centred);
    overdubLevelLabel.attachToComponent(&overdubLevelSlider, false);
    addAndMakeVisible(overdubLevelSlider);
    addAndMakeVisible(overdubLevelLabel);
    overdubLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "overdubLevel", overdubLevelSlider);

    configureSlider(erodeAmountSlider);
    erodeAmountSlider.setRange(0.0, 1.0, 0.001);
    erodeAmountLabel.setText("Erode", juce::dontSendNotification);
    erodeAmountLabel.setJustificationType(juce::Justification::centred);
    erodeAmountLabel.attachToComponent(&erodeAmountSlider, false);
    addAndMakeVisible(erodeAmountSlider);
    addAndMakeVisible(erodeAmountLabel);
    erodeAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "erodeAmount", erodeAmountSlider);

    configureSlider(filterSlider);
    filterSlider.setRange(0.0, 1.0, 0.001);
    filterLabel.setText("Filter", juce::dontSendNotification);
    filterLabel.setJustificationType(juce::Justification::centred);
    filterLabel.attachToComponent(&filterSlider, false);
    addAndMakeVisible(filterSlider);
    addAndMakeVisible(filterLabel);
    filterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "filter", filterSlider);

    configureSlider(noiseSlider);
    noiseSlider.setRange(0.0, 1.0, 0.001);
    noiseLabel.setText("Noise", juce::dontSendNotification);
    noiseLabel.setJustificationType(juce::Justification::centred);
    noiseLabel.attachToComponent(&noiseSlider, false);
    addAndMakeVisible(noiseSlider);
    addAndMakeVisible(noiseLabel);
    noiseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "noise", noiseSlider);

    configureSlider(modDepthSlider);
    modDepthSlider.setRange(0.0, 1.0, 0.001);
    modDepthLabel.setText("Mod Depth", juce::dontSendNotification);
    modDepthLabel.setJustificationType(juce::Justification::centred);
    modDepthLabel.attachToComponent(&modDepthSlider, false);
    addAndMakeVisible(modDepthSlider);
    addAndMakeVisible(modDepthLabel);
    modDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "modDepth", modDepthSlider);

    configureSlider(modSpeedSlider);
    modSpeedSlider.setRange(0.0, 1.0, 0.001);
    modSpeedLabel.setText("Mod Speed", juce::dontSendNotification);
    modSpeedLabel.setJustificationType(juce::Justification::centred);
    modSpeedLabel.attachToComponent(&modSpeedSlider, false);
    addAndMakeVisible(modSpeedSlider);
    addAndMakeVisible(modSpeedLabel);
    modSpeedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "modSpeed", modSpeedSlider);

    configureSlider(outputGainSlider);
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

    const int totalSliderWidth = kSliderWidth * kSliderCount + kSliderGap * (kSliderCount - 1);
    const int totalWidth = kLeftColumnWidth + totalSliderWidth + kRightPanelWidth + kMargin * 2;
    setSize(totalWidth, 360);
    startTimerHz(30);
}

SixteenSecondAudioProcessorEditor::~SixteenSecondAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void SixteenSecondAudioProcessorEditor::paint(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

void SixteenSecondAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(kMargin);
    auto header = area.removeFromTop(kHeaderHeight);

    auto leftColumn = area.removeFromLeft(kLeftColumnWidth);
    auto topRow = area.removeFromTop(kSliderHeight);
    auto sliderArea = topRow.removeFromLeft(kSliderWidth * kSliderCount + kSliderGap * (kSliderCount - 1));
    auto rightPanelArea = topRow.removeFromLeft(kRightPanelWidth);
    const auto leftPanelBounds = leftColumn;
    const auto mainPanelBounds = sliderArea;
    const auto rightPanelBounds = rightPanelArea;

    auto addSlider = [&](juce::Component& slider)
    {
        slider.setBounds(sliderArea.removeFromLeft(kSliderWidth).withTrimmedTop(8));
        sliderArea.removeFromLeft(kSliderGap);
    };

    addSlider(delayTimeSlider);
    addSlider(feedbackSlider);
    addSlider(mixSlider);
    addSlider(modDepthSlider);
    addSlider(modSpeedSlider);
    addSlider(filterSlider);
    addSlider(noiseSlider);
    addSlider(overdubLevelSlider);
    addSlider(erodeAmountSlider);

    auto outputArea = rightPanelArea.reduced(16, 14);
    outputGainSlider.setBounds(outputArea);

    auto buttonArea = leftColumn.removeFromTop(200);
    recordButton.setBounds(buttonArea.removeFromTop(32).reduced(8, 2));
    playButton.setBounds(buttonArea.removeFromTop(32).reduced(8, 2));
    overdubButton.setBounds(buttonArea.removeFromTop(32).reduced(8, 2));
    clearButton.setBounds(buttonArea.removeFromTop(32).reduced(8, 2));

    auto modeArea = leftColumn.removeFromTop(130);
    halfSpeedButton.setBounds(modeArea.removeFromTop(28).reduced(8, 2));
    reverseButton.setBounds(modeArea.removeFromTop(28).reduced(8, 2));
    authenticButton.setBounds(modeArea.removeFromTop(28).reduced(8, 2));
    limiterButton.setBounds(modeArea.removeFromTop(28).reduced(8, 2));

    auto ledArea = leftColumn.removeFromTop(32).reduced(8, 6);
    const auto recLed = ledArea.removeFromLeft(20);
    ledArea.removeFromLeft(8);
    const auto playLed = ledArea.removeFromLeft(20);
    ledArea.removeFromLeft(8);
    const auto overdubLed = ledArea.removeFromLeft(20);

    background.setBounds(getLocalBounds());
    background.setPanels(header.toFloat(),
                         leftPanelBounds.withTrimmedTop(4).withTrimmedBottom(6).toFloat(),
                         mainPanelBounds.withTrimmedTop(-8).withTrimmedBottom(20).toFloat(),
                         rightPanelBounds.withTrimmedTop(-8).withTrimmedBottom(20).toFloat());
    background.setMeterData(meterL, meterR);
}

void SixteenSecondAudioProcessorEditor::timerCallback()
{
    meterL = processor.getMeterL();
    meterR = processor.getMeterR();
    clipOn = processor.getClip();
    recOn = processor.getAPVTS().getRawParameterValue("record")->load() > 0.5f;
    playOn = processor.getAPVTS().getRawParameterValue("play")->load() > 0.5f;
    overdubOn = processor.getAPVTS().getRawParameterValue("overdub")->load() > 0.5f;
    background.setMeterData(meterL, meterR);
    repaint();
}
