#include <JuceHeader.h>
#include <cmath>
#include "PlotComponent.h"
#include "BaseColours.h"

//==============================================================================
PlotComponent::PlotComponent()
{

}

PlotComponent::~PlotComponent()
{
}

void PlotComponent::paint (juce::Graphics& g)
{
    using namespace juce;
    
    auto bounds = getLocalBounds();
    
    g.fillAll(BaseColours::darkPink);
    g.setColour(BaseColours::white);
    
    // Grid drawing
    
    constexpr int textFieldHeight = 25;
    int usefulHeight = bounds.getHeight() - 2 * textFieldHeight;
    
    drawGrid(g, 0, textFieldHeight, usefulHeight, 4);
    
    // Channel Label (Left/Right)
    
    constexpr int channelLabelPad = 3;
    int channelLabelY = textFieldHeight - channelLabelPad;
    
    g.drawSingleLineText("Left",
                         channelLabelPad,
                         channelLabelY);
    
    auto labelWidth = GlyphArrangement::getStringWidth(g.getCurrentFont(), "Right");
    g.drawSingleLineText("Right",
                         bounds.getWidth() - labelWidth - channelLabelPad,
                         channelLabelY);
    
    // Pan function plot
    
    Path plotPath;
    
    float scaleX = MathConstants<float>::twoPi / usefulHeight;
    const float centerY = bounds.getWidth() / 2;
    const float amplitude = -centerY + 2;
    
    for (int x = 0; x < bounds.getHeight(); x++)
    {
        auto fx = x * scaleX + 2 * textFieldHeight;
        auto y = std::sin(fx);
//        auto y = 2 * abs(2 * (fx / MathConstants<float>::twoPi - std::floor(fx / MathConstants<float>::twoPi + 0.5))) - 1;
        auto scaledY = centerY + y * amplitude;
        
        if (x == 0)
        {
            plotPath.startNewSubPath(scaledY, 0);
            continue;
        }
        
        plotPath.lineTo(scaledY, x);
    }
    
    g.strokePath(plotPath, PathStrokeType(4, PathStrokeType::curved, PathStrokeType::rounded));
}

void PlotComponent::resized()
{

}

void PlotComponent::drawGrid(juce::Graphics& g, int x, int y, int height, int linesNum)
{
    juce::Graphics::ScopedSaveState state(g);
    
    constexpr float lineThickness = 1.5;
    
    // horizontal lines
    int horizontaLinesNum = std::pow(linesNum, 3);
    float lineStep = height / horizontaLinesNum;
    
    bool isHighlightedSubline = true;
    
    for (int i = 0; i <= horizontaLinesNum; i++)
    {
        if (not (i % 4) or not (i % 16))
        {
            isHighlightedSubline = true;
            g.setColour(BaseColours::white);
        }
        
        g.fillRect(juce::Rectangle<float>(
                                    x,
                                    y + i * lineStep,
                                    getWidth(),
                                    lineThickness)
                   );
        
        if (isHighlightedSubline)
        {
            g.setColour(BaseColours::subLineColor);
            isHighlightedSubline = false;
        }
    }
    
    // vertical lines
    g.setColour(BaseColours::baseLineColor);
    lineStep = getWidth() / linesNum;
    
    for (int i = 1; i < linesNum; i++)
    {
        g.fillRect(juce::Rectangle<float>(
                                    x + i * lineStep,
                                    0,
                                    lineThickness,
                                    getHeight())
                   );
    }
    
    // vertical sublines
    g.setColour(BaseColours::subLineColor);
    
    lineStep /= linesNum;
    
    for (int i = 1; i < linesNum * linesNum; i++)
    {
        g.fillRect(juce::Rectangle<float>(
                                    x + i * lineStep,
                                    0,
                                    lineThickness,
                                    getHeight())
                   );
    }
}
