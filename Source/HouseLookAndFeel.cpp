#include "HouseLookAndFeel.h"

HouseLookAndFeel::HouseLookAndFeel()
{
    baseText = juce::Colour::fromRGB(230, 235, 245);
    cyan = juce::Colour::fromRGB(86, 220, 255);
    cyanBright = juce::Colour::fromRGB(122, 248, 255);
    panelDark = juce::Colour::fromRGB(17, 25, 38);
    panelLight = juce::Colour::fromRGB(42, 56, 76);

    setColour(juce::Label::textColourId, baseText);
    setColour(juce::TextEditor::textColourId, baseText);
    setColour(juce::TextEditor::highlightColourId, cyan.withAlpha(0.35f));
}

void HouseLookAndFeel::drawLinearSlider(juce::Graphics& g,
                                        int x, int y, int width, int height,
                                        float sliderPos, float minSliderPos, float maxSliderPos,
                                        const juce::Slider::SliderStyle style,
                                        juce::Slider& slider)
{
    if (style != juce::Slider::LinearVertical)
    {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        return;
    }

    const auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                               static_cast<float>(width), static_cast<float>(height));
    const auto track = bounds.withWidth(10.0f).withX(bounds.getCentreX() - 5.0f).reduced(0.0f, 6.0f);

    g.setColour(panelDark);
    g.fillRoundedRectangle(track, 5.0f);

    const auto fillTop = sliderPos;
    const auto filled = juce::Rectangle<float>(track.getX(), fillTop, track.getWidth(), track.getBottom() - fillTop);

    juce::ColourGradient grad(cyanBright, filled.getCentreX(), filled.getBottom(),
                              cyan, filled.getCentreX(), filled.getY(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(filled, 5.0f);

    g.setColour(cyan.withAlpha(0.4f));
    g.drawRoundedRectangle(track, 5.0f, 1.0f);

    const auto knobSize = 22.0f;
    const auto knob = juce::Rectangle<float>(knobSize, knobSize).withCentre({ track.getCentreX(), sliderPos });

    g.setColour(cyan.withAlpha(0.2f));
    g.fillEllipse(knob.expanded(6.0f));

    juce::ColourGradient knobGrad(cyanBright, knob.getCentreX(), knob.getY(),
                                  cyan, knob.getCentreX(), knob.getBottom(), false);
    g.setGradientFill(knobGrad);
    g.fillEllipse(knob);

    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.drawEllipse(knob, 1.0f);
}

void HouseLookAndFeel::drawToggleButton(juce::Graphics& g,
                                        juce::ToggleButton& button,
                                        bool, bool)
{
    auto bounds = button.getLocalBounds().toFloat();
    const auto boxSize = std::max(18.0f, bounds.getHeight() - 4.0f);
    const auto box = bounds.removeFromLeft(boxSize).withSizeKeepingCentre(boxSize, boxSize).reduced(2.0f);

    const auto on = button.getToggleState();
    const auto fill = on ? cyan.withAlpha(0.7f) : panelDark.withAlpha(0.8f);

    g.setColour(panelLight.withAlpha(0.35f));
    g.fillRoundedRectangle(box.expanded(2.0f), 6.0f);

    g.setColour(fill);
    g.fillRoundedRectangle(box, 6.0f);

    g.setColour(baseText);
    g.setFont(15.5f);
    g.drawText(button.getButtonText(), bounds.withTrimmedLeft(8), juce::Justification::centredLeft);
}

void HouseLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                            juce::Button& button,
                                            const juce::Colour&,
                                            bool shouldDrawButtonAsHighlighted,
                                            bool shouldDrawButtonAsDown)
{
    const auto bounds = button.getLocalBounds().toFloat().reduced(2.0f);
    const auto radius = bounds.getHeight() * 0.5f;

    juce::ColourGradient grad(panelLight, bounds.getX(), bounds.getY(),
                              panelDark, bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(bounds, radius);

    if (shouldDrawButtonAsHighlighted || shouldDrawButtonAsDown)
    {
        g.setColour(cyan.withAlpha(0.25f));
        g.fillRoundedRectangle(bounds, radius);
    }

    g.setColour(cyan.withAlpha(0.5f));
    g.drawRoundedRectangle(bounds, radius, 1.0f);
}

void HouseLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.setColour(label.findColour(juce::Label::textColourId));
    g.setFont(label.getFont());
    g.drawFittedText(label.getText(), label.getLocalBounds(), label.getJustificationType(), 1);
}

void HouseLookAndFeel::fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor&)
{
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    g.setColour(panelDark.withAlpha(0.9f));
    g.fillRoundedRectangle(bounds.reduced(1.0f), 6.0f);
}

void HouseLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor&)
{
    auto bounds = juce::Rectangle<float>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    g.setColour(panelLight.withAlpha(0.6f));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 6.0f, 1.0f);
}
