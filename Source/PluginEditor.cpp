#include "PluginEditor.h"

namespace
{
    constexpr int kSliderWidth = 90;
    constexpr int kSliderHeight = 170;

    void configureSlider(juce::Slider& slider)
    {
        slider.setSliderStyle(juce::Slider::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    }
}

SixteenSecondAudioProcessorEditor::SixteenSecondAudioProcessorEditor(SixteenSecondAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
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

    setSize(1220, 360);
    startTimerHz(30);
}

SixteenSecondAudioProcessorEditor::~SixteenSecondAudioProcessorEditor() = default;

void SixteenSecondAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds();
    juce::Colour base = juce::Colour::fromRGB(18, 18, 20);
    juce::Colour panel = juce::Colour::fromRGB(32, 32, 36);
    juce::Colour accent = juce::Colour::fromRGB(231, 168, 74);
    juce::Colour text = juce::Colour::fromRGB(228, 228, 228);

    g.fillAll(base);

    auto header = bounds.removeFromTop(48);
    g.setColour(panel);
    g.fillRect(header);

    g.setColour(accent);
    g.setFont(juce::Font("Georgia", 20.0f, juce::Font::bold));
    g.drawText("16-Second", header.removeFromLeft(200), juce::Justification::centredLeft);

    g.setColour(text);
    g.setFont(juce::Font("Georgia", 12.0f, juce::Font::italic));
    g.drawText("unsafe digital delay/looper", header.removeFromLeft(220), juce::Justification::centredLeft);

    g.setColour(panel);
    g.fillRoundedRectangle(meterArea.toFloat(), 6.0f);

    const auto meterHeight = meterArea.getHeight() - 8;
    const auto meterWidth = meterArea.getWidth() / 2 - 10;
    const auto baseY = meterArea.getBottom() - 4;

    const auto filledL = static_cast<int>(meterHeight * juce::jlimit(0.0f, 1.0f, meterL));
    const auto filledR = static_cast<int>(meterHeight * juce::jlimit(0.0f, 1.0f, meterR));

    const auto leftMeter = juce::Rectangle<int>(meterArea.getX() + 6,
                                                baseY - filledL,
                                                meterWidth,
                                                filledL);
    const auto rightMeter = juce::Rectangle<int>(meterArea.getX() + meterWidth + 14,
                                                 baseY - filledR,
                                                 meterWidth,
                                                 filledR);

    g.setColour(juce::Colour::fromRGB(82, 214, 125));
    g.fillRect(leftMeter);
    g.fillRect(rightMeter);

    g.setColour(clipOn ? juce::Colours::red : juce::Colour::fromRGB(80, 20, 20));
    g.fillEllipse(clipLedArea.toFloat());

    auto drawLed = [&](const juce::Rectangle<int>& area, juce::Colour onColour, bool isOn)
    {
        g.setColour(isOn ? onColour : onColour.darker(0.6f));
        g.fillEllipse(area.toFloat());
    };

    drawLed(recLedArea, juce::Colour::fromRGB(220, 60, 60), recOn);
    drawLed(playLedArea, juce::Colour::fromRGB(60, 220, 120), playOn);
    drawLed(overdubLedArea, juce::Colour::fromRGB(230, 170, 50), overdubOn);
}

void SixteenSecondAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(16);
    area.removeFromTop(48);

    auto leftColumn = area.removeFromLeft(240);
    auto sliderArea = area.removeFromTop(kSliderHeight);

    auto addSlider = [&](juce::Component& slider)
    {
        slider.setBounds(sliderArea.removeFromLeft(kSliderWidth).withTrimmedTop(8));
        sliderArea.removeFromLeft(10);
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
    addSlider(outputGainSlider);

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
    recLedArea = ledArea.removeFromLeft(20);
    ledArea.removeFromLeft(8);
    playLedArea = ledArea.removeFromLeft(20);
    ledArea.removeFromLeft(8);
    overdubLedArea = ledArea.removeFromLeft(20);

    meterArea = area.removeFromRight(80).withTrimmedTop(10).withTrimmedBottom(10);
    const auto led = meterArea.removeFromTop(16);
    clipLedArea = led.withSizeKeepingCentre(12, 12);
}

void SixteenSecondAudioProcessorEditor::timerCallback()
{
    meterL = processor.getMeterL();
    meterR = processor.getMeterR();
    clipOn = processor.getClip();
    recOn = processor.getAPVTS().getRawParameterValue("record")->load() > 0.5f;
    playOn = processor.getAPVTS().getRawParameterValue("play")->load() > 0.5f;
    overdubOn = processor.getAPVTS().getRawParameterValue("overdub")->load() > 0.5f;
    repaint();
}
