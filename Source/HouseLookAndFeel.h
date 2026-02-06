#pragma once

#include <JuceHeader.h>

class HouseLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    HouseLookAndFeel();

    void drawLinearSlider(juce::Graphics&,
                          int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle,
                          juce::Slider&) override;

    void drawToggleButton(juce::Graphics&,
                          juce::ToggleButton&,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;

    void drawButtonBackground(juce::Graphics&,
                              juce::Button&,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    void drawLabel(juce::Graphics&, juce::Label&) override;
    void fillTextEditorBackground(juce::Graphics&, int width, int height, juce::TextEditor&) override;
    void drawTextEditorOutline(juce::Graphics&, int width, int height, juce::TextEditor&) override;

private:
    juce::Colour baseText;
    juce::Colour cyan;
    juce::Colour cyanBright;
    juce::Colour panelDark;
    juce::Colour panelLight;
};
