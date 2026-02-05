#include "PluginEditor.h"

#include <cmath>

namespace
{
    constexpr int kSliderWidth = 80;
    constexpr int kSliderHeight = 170;
    constexpr int kSliderGap = 6;
    constexpr int kSliderCount = 10;
    constexpr int kLeftColumnWidth = 210;
    constexpr int kMeterWidth = 70;
    constexpr int kMargin = 16;
    constexpr int kHeaderHeight = 48;

    void configureSlider(juce::Slider& slider)
    {
        slider.setSliderStyle(juce::Slider::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    }
}

SurfaceComponent::SurfaceComponent()
{
    addAndMakeVisible(content);
    constexpr double kUiRate = 60.0;
    constexpr double kSmoothingSeconds = 0.25;

    posture.reset(kUiRate, kSmoothingSeconds);
    tilt.reset(kUiRate, kSmoothingSeconds);
    jitter.reset(kUiRate, kSmoothingSeconds);
    danger.reset(kUiRate, kSmoothingSeconds);
}

void SurfaceComponent::setTargets(const VisualStateTargets& newTargets)
{
    targets = newTargets;
    posture.setTargetValue(targets.postureCompression);
    tilt.setTargetValue(targets.tiltRadians);
    jitter.setTargetValue(targets.jitterAmount);
    danger.setTargetValue(targets.dangerAmount);
}

void SurfaceComponent::setDebug(bool shouldExaggerate)
{
    debug = shouldExaggerate;
}

void SurfaceComponent::tick()
{
    updateTransform();
    repaint();
}

void SurfaceComponent::setMeterData(float newMeterL, float newMeterR, bool newClip,
                                    bool newRec, bool newPlay, bool newOverdub)
{
    meterL = newMeterL;
    meterR = newMeterR;
    clipOn = newClip;
    recOn = newRec;
    playOn = newPlay;
    overdubOn = newOverdub;
}

void SurfaceComponent::setVisualBounds(const juce::Rectangle<int>& newMeterArea,
                                       const juce::Rectangle<int>& newClipLed,
                                       const juce::Rectangle<int>& newRecLed,
                                       const juce::Rectangle<int>& newPlayLed,
                                       const juce::Rectangle<int>& newOverdubLed)
{
    meterArea = newMeterArea;
    clipLedArea = newClipLed;
    recLedArea = newRecLed;
    playLedArea = newPlayLed;
    overdubLedArea = newOverdubLed;
}

void SurfaceComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    juce::Colour base = juce::Colour::fromRGB(18, 18, 20);
    juce::Colour panel = juce::Colour::fromRGB(32, 32, 36);
    juce::Colour accent = juce::Colour::fromRGB(231, 168, 74);
    juce::Colour text = juce::Colour::fromRGB(228, 228, 228);

    g.fillAll(base);

    g.addTransform(currentTransform);

    auto header = bounds.removeFromTop(kHeaderHeight);
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

void SurfaceComponent::updateTransform()
{
    const auto postureValue = posture.getNextValue();
    const auto tiltValue = tilt.getNextValue();
    const auto jitterValue = jitter.getNextValue();
    const auto dangerValue = danger.getNextValue();

    const auto exaggeration = debug ? 3.0f : 1.0f;
    const auto compression = juce::jlimit(0.0f, 0.12f, postureValue * 0.06f * exaggeration);
    const auto tiltRadians = juce::jlimit(-0.08f, 0.08f, tiltValue * exaggeration);
    const auto jitterPixels = juce::jlimit(0.0f, 4.0f, jitterValue * 2.0f * exaggeration);

    jitterPhase += 0.12f + dangerValue * 0.08f;

    const auto jitterX = std::sin(jitterPhase) * jitterPixels;
    const auto jitterY = std::cos(jitterPhase * 1.31f) * jitterPixels;

    const auto bounds = content.getBounds().toFloat();
    const auto centre = bounds.getCentre();

    currentTransform = juce::AffineTransform::translation(-centre.x, -centre.y)
                           .scaled(1.0f, 1.0f - compression)
                           .rotated(tiltRadians)
                           .translated(centre.x + jitterX, centre.y + jitterY);

    content.setTransform(currentTransform);
}

SixteenSecondAudioProcessorEditor::SixteenSecondAudioProcessorEditor(SixteenSecondAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setWantsKeyboardFocus(true);
    addAndMakeVisible(surface);
    auto& content = surface.getContent();
    configureSlider(delayTimeSlider);
    delayTimeSlider.setRange(0.0, 16000.0, 1.0);
    delayTimeLabel.setText("Delay", juce::dontSendNotification);
    delayTimeLabel.setJustificationType(juce::Justification::centred);
    delayTimeLabel.attachToComponent(&delayTimeSlider, false);
    content.addAndMakeVisible(delayTimeSlider);
    content.addAndMakeVisible(delayTimeLabel);
    delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "delayTime", delayTimeSlider);

    configureSlider(feedbackSlider);
    feedbackSlider.setRange(0.0, 1.2, 0.001);
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    feedbackLabel.attachToComponent(&feedbackSlider, false);
    content.addAndMakeVisible(feedbackSlider);
    content.addAndMakeVisible(feedbackLabel);
    feedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "feedback", feedbackSlider);

    configureSlider(mixSlider);
    mixSlider.setRange(0.0, 1.0, 0.001);
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.attachToComponent(&mixSlider, false);
    content.addAndMakeVisible(mixSlider);
    content.addAndMakeVisible(mixLabel);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "mix", mixSlider);

    configureSlider(overdubLevelSlider);
    overdubLevelSlider.setRange(0.0, 1.0, 0.001);
    overdubLevelLabel.setText("Overdub", juce::dontSendNotification);
    overdubLevelLabel.setJustificationType(juce::Justification::centred);
    overdubLevelLabel.attachToComponent(&overdubLevelSlider, false);
    content.addAndMakeVisible(overdubLevelSlider);
    content.addAndMakeVisible(overdubLevelLabel);
    overdubLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "overdubLevel", overdubLevelSlider);

    configureSlider(erodeAmountSlider);
    erodeAmountSlider.setRange(0.0, 1.0, 0.001);
    erodeAmountLabel.setText("Erode", juce::dontSendNotification);
    erodeAmountLabel.setJustificationType(juce::Justification::centred);
    erodeAmountLabel.attachToComponent(&erodeAmountSlider, false);
    content.addAndMakeVisible(erodeAmountSlider);
    content.addAndMakeVisible(erodeAmountLabel);
    erodeAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "erodeAmount", erodeAmountSlider);

    configureSlider(filterSlider);
    filterSlider.setRange(0.0, 1.0, 0.001);
    filterLabel.setText("Filter", juce::dontSendNotification);
    filterLabel.setJustificationType(juce::Justification::centred);
    filterLabel.attachToComponent(&filterSlider, false);
    content.addAndMakeVisible(filterSlider);
    content.addAndMakeVisible(filterLabel);
    filterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "filter", filterSlider);

    configureSlider(noiseSlider);
    noiseSlider.setRange(0.0, 1.0, 0.001);
    noiseLabel.setText("Noise", juce::dontSendNotification);
    noiseLabel.setJustificationType(juce::Justification::centred);
    noiseLabel.attachToComponent(&noiseSlider, false);
    content.addAndMakeVisible(noiseSlider);
    content.addAndMakeVisible(noiseLabel);
    noiseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "noise", noiseSlider);

    configureSlider(modDepthSlider);
    modDepthSlider.setRange(0.0, 1.0, 0.001);
    modDepthLabel.setText("Mod Depth", juce::dontSendNotification);
    modDepthLabel.setJustificationType(juce::Justification::centred);
    modDepthLabel.attachToComponent(&modDepthSlider, false);
    content.addAndMakeVisible(modDepthSlider);
    content.addAndMakeVisible(modDepthLabel);
    modDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "modDepth", modDepthSlider);

    configureSlider(modSpeedSlider);
    modSpeedSlider.setRange(0.0, 1.0, 0.001);
    modSpeedLabel.setText("Mod Speed", juce::dontSendNotification);
    modSpeedLabel.setJustificationType(juce::Justification::centred);
    modSpeedLabel.attachToComponent(&modSpeedSlider, false);
    content.addAndMakeVisible(modSpeedSlider);
    content.addAndMakeVisible(modSpeedLabel);
    modSpeedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "modSpeed", modSpeedSlider);

    configureSlider(outputGainSlider);
    outputGainSlider.setRange(-24.0, 12.0, 0.01);

    outputGainLabel.setText("Output", juce::dontSendNotification);
    outputGainLabel.setJustificationType(juce::Justification::centred);
    outputGainLabel.attachToComponent(&outputGainSlider, false);

    content.addAndMakeVisible(outputGainSlider);
    content.addAndMakeVisible(outputGainLabel);

    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "outputGain", outputGainSlider);

    recordButton.setButtonText("Record");
    playButton.setButtonText("Play");
    content.addAndMakeVisible(recordButton);
    content.addAndMakeVisible(playButton);

    recordAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "record", recordButton);
    playAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "play", playButton);

    overdubButton.setButtonText("Overdub");
    clearButton.setButtonText("Clear");
    clearButton.setClickingTogglesState(true);
    content.addAndMakeVisible(overdubButton);
    content.addAndMakeVisible(clearButton);

    overdubAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "overdub", overdubButton);
    clearAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "clear", clearButton);

    halfSpeedButton.setButtonText("Half");
    reverseButton.setButtonText("Reverse");
    content.addAndMakeVisible(halfSpeedButton);
    content.addAndMakeVisible(reverseButton);

    halfSpeedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "halfSpeed", halfSpeedButton);
    reverseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "reverse", reverseButton);

    authenticButton.setButtonText("Authentic");
    content.addAndMakeVisible(authenticButton);

    authenticAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "authentic", authenticButton);

    limiterButton.setButtonText("Limiter");
    content.addAndMakeVisible(limiterButton);

    limiterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "limiter", limiterButton);

    const int totalSliderWidth = kSliderWidth * kSliderCount + kSliderGap * (kSliderCount - 1);
    const int totalWidth = kLeftColumnWidth + totalSliderWidth + kMeterWidth + kMargin * 2;
    setSize(totalWidth, 360);
    startTimerHz(30);
}

SixteenSecondAudioProcessorEditor::~SixteenSecondAudioProcessorEditor() = default;

void SixteenSecondAudioProcessorEditor::paint(juce::Graphics&)
{
}

void SixteenSecondAudioProcessorEditor::resized()
{
    surface.setBounds(getLocalBounds());
    surface.getContent().setBounds(surface.getLocalBounds());

    auto area = surface.getLocalBounds().reduced(kMargin);
    area.removeFromTop(kHeaderHeight);

    auto leftColumn = area.removeFromLeft(kLeftColumnWidth);
    auto topRow = area.removeFromTop(kSliderHeight);
    auto sliderArea = topRow.removeFromLeft(kSliderWidth * kSliderCount + kSliderGap * (kSliderCount - 1));

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
    const auto recLed = ledArea.removeFromLeft(20);
    ledArea.removeFromLeft(8);
    const auto playLed = ledArea.removeFromLeft(20);
    ledArea.removeFromLeft(8);
    const auto overdubLed = ledArea.removeFromLeft(20);

    auto meter = topRow.removeFromLeft(kMeterWidth).withTrimmedTop(10).withTrimmedBottom(10);
    const auto led = meter.removeFromTop(16);
    surface.setVisualBounds(meter, led.withSizeKeepingCentre(12, 12), recLed, playLed, overdubLed);
}

void SixteenSecondAudioProcessorEditor::timerCallback()
{
    const auto isRecord = processor.getAPVTS().getRawParameterValue("record")->load() > 0.5f;
    const auto isPlay = processor.getAPVTS().getRawParameterValue("play")->load() > 0.5f;
    const auto isOverdub = processor.getAPVTS().getRawParameterValue("overdub")->load() > 0.5f;
    const auto isAuthentic = processor.getAPVTS().getRawParameterValue("authentic")->load() > 0.5f;
    const auto feedback = processor.getAPVTS().getRawParameterValue("feedback")->load();

    VisualStateTargets targets;
    targets.postureCompression = (isRecord ? 0.35f : 0.0f) + (isOverdub ? 0.2f : 0.0f) + (isPlay ? 0.08f : 0.0f);
    targets.postureCompression = juce::jlimit(0.0f, 1.0f, targets.postureCompression);

    // Keep mapping conservative: authentic adds a subtle tilt, feedback adds slight danger.
    targets.dangerAmount = juce::jlimit(0.0f, 1.0f, (feedback - 0.8f) / 0.4f);
    targets.tiltRadians = (isAuthentic ? 0.02f : 0.0f) + targets.dangerAmount * 0.01f;
    targets.jitterAmount = juce::jlimit(0.0f, 1.0f, targets.dangerAmount + (isOverdub ? 0.1f : 0.0f));

    surface.setTargets(targets);
    surface.setMeterData(processor.getMeterL(), processor.getMeterR(), processor.getClip(),
                         isRecord, isPlay, isOverdub);
    surface.tick();
    return;
}

bool SixteenSecondAudioProcessorEditor::keyPressed(const juce::KeyPress& key)
{
    if (key.getTextCharacter() == 'd' || key.getTextCharacter() == 'D')
    {
        debugMorph = !debugMorph;
        surface.setDebug(debugMorph);
        return true;
    }
    return false;
}
