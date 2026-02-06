#include "BackgroundWavesComponent.h"

#include <cmath>

BackgroundWavesComponent::BackgroundWavesComponent()
{
    startTimerHz(30);
}

void BackgroundWavesComponent::setPanels(const juce::Rectangle<float>& header,
                                         const juce::Rectangle<float>& left,
                                         const juce::Rectangle<float>& main,
                                         const juce::Rectangle<float>& right)
{
    headerPanel = header;
    leftPanel = left;
    mainPanel = main;
    rightPanel = right;
    repaint();
}

void BackgroundWavesComponent::setAnimationEnabled(bool enabled)
{
    animate = enabled;
}

void BackgroundWavesComponent::setMeterData(float left, float right)
{
    meterL = left;
    meterR = right;
}

void BackgroundWavesComponent::paint(juce::Graphics& g)
{
    g.drawImageAt(background, 0, 0);
    drawWaves(g, getLocalBounds().toFloat());

    drawGlassPanel(g, headerPanel, 18.0f);
    drawGlassPanel(g, leftPanel, 18.0f);
    drawGlassPanel(g, mainPanel, 18.0f);
    drawGlassPanel(g, rightPanel, 18.0f);

    if (!headerPanel.isEmpty())
    {
        auto header = headerPanel.reduced(18.0f, 10.0f);
        g.setColour(juce::Colour::fromRGB(235, 240, 250));
        g.setFont(juce::Font("Georgia", 26.0f, juce::Font::bold));
        g.drawText("16-Second", header.removeFromLeft(260.0f), juce::Justification::centredLeft);

        g.setColour(juce::Colour::fromRGB(195, 205, 220));
        g.setFont(juce::Font("Georgia", 15.0f, juce::Font::plain));
        g.drawText("unsafe digital delay/looper", header, juce::Justification::centredLeft);
    }

    // Simple meter behind the output slider.
    auto meterArea = rightPanel.reduced(10.0f, 24.0f);
    meterArea.setWidth(32.0f);
    const auto meterHeight = meterArea.getHeight();
    const auto filledL = meterHeight * juce::jlimit(0.0f, 1.0f, meterL);
    const auto filledR = meterHeight * juce::jlimit(0.0f, 1.0f, meterR);

    g.setColour(juce::Colour::fromRGB(45, 70, 90));
    g.fillRoundedRectangle(meterArea, 6.0f);

    const auto barWidth = (meterArea.getWidth() - 6.0f) / 2.0f;
    const auto leftBar = juce::Rectangle<float>(meterArea.getX() + 3.0f,
                                                meterArea.getBottom() - filledL,
                                                barWidth - 1.0f,
                                                filledL);
    const auto rightBar = juce::Rectangle<float>(meterArea.getX() + barWidth + 3.0f,
                                                 meterArea.getBottom() - filledR,
                                                 barWidth - 1.0f,
                                                 filledR);

    g.setColour(juce::Colour::fromRGB(90, 226, 255));
    g.fillRoundedRectangle(leftBar, 4.0f);
    g.fillRoundedRectangle(rightBar, 4.0f);
}

void BackgroundWavesComponent::resized()
{
    rebuildBackground();
}

void BackgroundWavesComponent::timerCallback()
{
    if (!animate)
        return;

    phase += 0.015f;
    repaint();
}

void BackgroundWavesComponent::rebuildBackground()
{
    const auto bounds = getLocalBounds();
    background = juce::Image(juce::Image::RGB, bounds.getWidth(), bounds.getHeight(), true);

    juce::Graphics g(background);
    juce::ColourGradient grad(juce::Colour::fromRGB(10, 18, 30), 0.0f, 0.0f,
                              juce::Colour::fromRGB(5, 8, 16), 0.0f, static_cast<float>(bounds.getHeight()), false);
    g.setGradientFill(grad);
    g.fillAll();
}

void BackgroundWavesComponent::drawGlassPanel(juce::Graphics& g, juce::Rectangle<float> bounds, float radius)
{
    if (bounds.isEmpty())
        return;

    juce::ColourGradient panelGrad(juce::Colour::fromRGBA(26, 34, 48, 180),
                                   bounds.getX(), bounds.getY(),
                                   juce::Colour::fromRGBA(15, 20, 32, 210),
                                   bounds.getX(), bounds.getBottom(), false);
    g.setGradientFill(panelGrad);
    g.fillRoundedRectangle(bounds, radius);

    g.setColour(juce::Colour::fromRGBA(110, 160, 190, 60));
    g.drawRoundedRectangle(bounds.expanded(1.0f), radius + 1.0f, 1.0f);

    g.setColour(juce::Colour::fromRGBA(180, 220, 255, 40));
    g.drawRoundedRectangle(bounds.reduced(1.0f), radius - 1.0f, 1.0f);
}

void BackgroundWavesComponent::drawWaves(juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    const float width = bounds.getWidth();
    const float height = bounds.getHeight();

    auto drawLayer = [&](float amplitude, float yOffset, float alpha)
    {
        juce::Path wave;
        wave.startNewSubPath(bounds.getX(), bounds.getY() + yOffset);
        for (int i = 0; i <= 120; ++i)
        {
            const float t = static_cast<float>(i) / 120.0f;
            const float x = bounds.getX() + t * width;
            const float y = bounds.getY() + yOffset + std::sin(t * 6.283f * 1.2f + phase) * amplitude;
            wave.lineTo(x, y);
        }

        g.setColour(juce::Colour::fromRGBA(40, 120, 160, static_cast<juce::uint8>(alpha)));
        g.strokePath(wave, juce::PathStrokeType(1.8f));
    };

    drawLayer(18.0f, height * 0.55f, 45.0f);
    drawLayer(12.0f, height * 0.65f, 35.0f);
    drawLayer(8.0f, height * 0.42f, 25.0f);
}
