#pragma once

#include <JuceHeader.h>

class BackgroundWavesComponent final : public juce::Component,
                                       private juce::Timer
{
public:
    BackgroundWavesComponent();

    void setPanels(const juce::Rectangle<float>& header,
                   const juce::Rectangle<float>& left,
                   const juce::Rectangle<float>& main,
                   const juce::Rectangle<float>& right);

    void setAnimationEnabled(bool enabled);
    void setMeterData(float left, float right);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void rebuildBackground();
    void drawGlassPanel(juce::Graphics& g, juce::Rectangle<float> bounds, float radius);
    void drawWaves(juce::Graphics& g, const juce::Rectangle<float>& bounds);

    juce::Image background;
    juce::Rectangle<float> headerPanel;
    juce::Rectangle<float> leftPanel;
    juce::Rectangle<float> mainPanel;
    juce::Rectangle<float> rightPanel;
    float phase = 0.0f;
    bool animate = true;
    float meterL = 0.0f;
    float meterR = 0.0f;
};
