#include "RotarySlider.h"

// RotarySlider
//==============================================================================

void RotarySliderWithLabels::paint(juce::Graphics &g)
{
    using namespace juce;

    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;

    auto range = getRange();
    auto sliderBounds = getSliderBounds();

    getLookAndFeel().drawRotarySlider(g, sliderBounds.getX(), sliderBounds.getY(),
                                      sliderBounds.getWidth(), sliderBounds.getHeight(),
                                      jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                      startAng, endAng, *this);
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    size -= size * 0.5;

    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), bounds.getCentreY() - 15);

    return r;
}

bool RotarySliderWithLabels::hitTest(int x, int y)
{
    juce::Point<float> mousePos(x, y);
    juce::Point<float> center = getSliderBounds().getCentre().toFloat();
    float radius = getSliderBounds().getWidth() / 2;

    return mousePos.getDistanceFrom(center) <= radius;
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    juce::String str;

    if (auto *floatParam = dynamic_cast<juce::AudioParameterFloat *>(param))
        str << param->name.toUpperCase() << "\n" << getValue() << suffix;
    else
        jassertfalse;

    return str;
}

// RotarySliderLookAndFeel
//==============================================================================

void RotarySliderLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width,
                                               int height, float sliderPosProportional,
                                               float rotaryStartAngle, float rotaryEndAngle,
                                               juce::Slider &slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);

    Path p;
    auto center = bounds.getCentre();

    // Yellow arc drawing
    constexpr float borderThickness = 5;
    const auto arcRadius = bounds.getHeight() / 2;

    p.addCentredArc(center.getX(), center.getY(), arcRadius, arcRadius, 0.f, rotaryStartAngle,
                    rotaryEndAngle, true);
    g.setColour(BaseColours::yellow);
    g.strokePath(
        p, juce::PathStrokeType(borderThickness, PathStrokeType::curved, PathStrokeType::rounded));

    // Main circle drawing
    g.setColour(BaseColours::basePink);
    g.fillEllipse(bounds.reduced(borderThickness));

    // White border drawing
    g.setColour(BaseColours::white);
    g.drawEllipse(bounds.reduced(borderThickness), 5);

    if (auto *rswl = dynamic_cast<RotarySliderWithLabels *>(&slider))
    {
        p.clear();

        // Notch drawing
        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.reduced(borderThickness).getY());
        r.setBottom(center.getY());

        p.addRectangle(r);

        // Slider rotation drawing
        jassert(rotaryStartAngle < rotaryEndAngle);

        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);

        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));

        g.setColour(BaseColours::white);
        g.fillPath(p);

        drawRotarySliderLabel(g, bounds, rswl->getDisplayString());
    }
}

void RotarySliderLookAndFeel::drawRotarySliderLabel(juce::Graphics &g,
                                                    juce::Rectangle<float> bounds,
                                                    const juce::String &displayString)
{
    juce::Graphics::ScopedSaveState state(g);

    static juce::FontOptions comicNeueOptions{typeface->getName(), 15, juce::Font::bold};
    auto font = juce::Font(comicNeueOptions);

    g.setColour(BaseColours::white);

    juce::StringArray lines;
    lines.addLines(displayString);
    float maxLineWidth = 0;

    for (auto line : lines)
    {
        float lineWidth = juce::GlyphArrangement::getStringWidth(font, line);
        if (lineWidth > maxLineWidth)
        {
            maxLineWidth = lineWidth;
        }
    }

    juce::GlyphArrangement ga;
    ga.addJustifiedText(font, displayString, bounds.getCentre().getX() - maxLineWidth / 2,
                        bounds.getBottom() + comicNeueOptions.getHeight(), maxLineWidth,
                        juce::Justification::centred);
    ga.draw(g);
}
