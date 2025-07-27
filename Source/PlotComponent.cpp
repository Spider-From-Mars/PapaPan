#include <JuceHeader.h>
#include <cmath>
#include "PlotComponent.h"
#include "BaseColours.h"

//==============================================================================
PlotComponent::PlotComponent(juce::AudioProcessorValueTreeState& apvts) :
    sinButton("Sin", juce::DrawableButton::ImageFitted),
    triangleButton("Triangle", juce::DrawableButton::ImageFitted)
{
    waveFunction = std::sin;
    
    auto sineSVG = juce::Drawable::createFromImageData(BinaryData::sinWave_svg,
                                                       BinaryData::sinWave_svgSize);
    sinButton.setImages(sineSVG.get());
    
    auto triangleSVG = juce::Drawable::createFromImageData(BinaryData::triangleWave_svg,
                                                           BinaryData::triangleWave_svgSize);
    triangleButton.setImages(triangleSVG.get());
    
    auto* waveParam = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("WAVETYPE"));
    setupWaveButton(sinButton, waveParam, Panner::waveType::sin);
    setupWaveButton(triangleButton, waveParam, Panner::waveType::triangle);
    
    sinButton.setToggleState(true, juce::dontSendNotification);
    sinButton.setRadioGroupId(1);
    triangleButton.setRadioGroupId(1);
}

PlotComponent::~PlotComponent()
{
}

void PlotComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    g.fillAll(BaseColours::darkPink);
    g.setColour(BaseColours::white);
    
    // Grid drawing
    
    constexpr int textFieldHeight = 25;
    int usefulHeight = bounds.getHeight() - 2 * textFieldHeight;
    
    drawGrid(g, 0, textFieldHeight, usefulHeight, 4);
    
    // Pan function plot
    drawPlot(g, usefulHeight);
    
    // Channel Label (Left/Right)
    
    constexpr int channelLabelPad = 3;
    int channelLabelY = textFieldHeight - channelLabelPad;
    
    g.setColour(BaseColours::transparentPink);
    g.fillRect(0, 0, bounds.getWidth(), textFieldHeight);
    g.fillRect(0,
               bounds.getBottom() - textFieldHeight,
               bounds.getWidth(),
               textFieldHeight
            );
    
    g.setColour(BaseColours::white);
    g.drawSingleLineText("Left",
                         channelLabelPad,
                         channelLabelY);
    
    auto labelWidth = juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), "Right");
    g.drawSingleLineText("Right",
                         bounds.getWidth() - labelWidth - channelLabelPad,
                         channelLabelY);
}

void PlotComponent::resized()
{
//    auto bounds = getLocalBounds();
    constexpr int buttonWidth = 25;
    sinButton.setBounds(10,
                        getLocalBounds().getBottom() - buttonWidth,
                        buttonWidth,
                        buttonWidth);
    triangleButton.setBounds(sinButton.getX() + buttonWidth + 5,
                             sinButton.getY(),
                             buttonWidth,
                             buttonWidth);
}

void PlotComponent::drawPlot(juce::Graphics& g, int xSize) const
{
    using namespace juce;
    Graphics::ScopedSaveState state(g);
    Path plotPath;
    
    auto bounds = getLocalBounds();
    
    float scaleX = MathConstants<float>::twoPi / xSize;
    const float centerY = bounds.getWidth() / 2;
    const float amplitude = -centerY + 2;
    
    for (int x = 0; x < bounds.getHeight(); x++)
    {
        auto fx = x * scaleX + (bounds.getHeight() - xSize);
        auto y = waveFunction(fx);
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

float PlotComponent::triangle(float x)
{
    auto period = juce::MathConstants<float>::twoPi;
    
    x = std::fmod(x + period / 4.0f, period) / period;
    
    return 1 - 4.0f * std::abs(x - 0.5f);
}

void PlotComponent::drawGrid(juce::Graphics& g, int x, int y, int height, int linesNum) const
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


void PlotComponent::setupWaveButton(juce::Button& button, juce::AudioParameterChoice* waveParam, Panner::waveType type)
{
    button.setColour(juce::DrawableButton::backgroundColourId, juce::Colours::transparentBlack);
    button.setColour(juce::DrawableButton::backgroundOnColourId,
                     BaseColours::darkPink);
    
    button.onClick = [this, waveParam, type]()
    {
        waveParam->beginChangeGesture();
        waveParam->setValueNotifyingHost(static_cast<int>(type));
        waveParam->endChangeGesture();
        
        switch (type) {
            case Panner::waveType::sin:
                waveFunction = std::sin;
                sinButton.setToggleState(true, juce::dontSendNotification);
                break;
            case Panner::waveType::triangle:
                waveFunction = triangle;
                triangleButton.setToggleState(true, juce::dontSendNotification);
                break;
            default:
                jassertfalse;
        }
        
        repaint(getLocalBounds());
    };
    
    addAndMakeVisible(button);
}
