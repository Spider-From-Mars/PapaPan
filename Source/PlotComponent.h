#pragma once

#include <JuceHeader.h>
#include "Panner.h"

class PlotComponent : public juce::Component, private juce::Timer
{
public:
    PlotComponent(juce::AudioProcessorValueTreeState& apvts, Modulation& mod);
    ~PlotComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlotComponent)
    
    void timerCallback() override { repaint(); }
    
    void drawGrid(juce::Graphics& g, int x, int y, int height, int linesNum) const;
    void drawPlot(juce::Graphics& g, int xSize) const;
    void drawPanSegment(juce::Graphics& g, int xSize) const;
    
    void setupWaveButton(juce::Button& button, juce::AudioParameterChoice* waveParam, Panner::waveType type);
    
    Modulation &mod;
    
    juce::DrawableButton sinButton;
    juce::DrawableButton triangleButton;
    
    float (*waveFunction)(float);
};
