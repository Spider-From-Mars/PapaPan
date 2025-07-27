#pragma once

#include <JuceHeader.h>
#include "Panner.h"

class PlotComponent : public juce::Component
{
public:
    PlotComponent(juce::AudioProcessorValueTreeState& apvts);
    ~PlotComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlotComponent)
    
    static float triangle(float x);
    
    void drawGrid(juce::Graphics& g, int x, int y, int height, int linesNum) const;
    void drawPlot(juce::Graphics& g, int xSize) const;
    
    void setupWaveButton(juce::Button& button, juce::AudioParameterChoice* waveParam, Panner::waveType type);
    
    juce::DrawableButton sinButton;
    juce::DrawableButton triangleButton;
    
    float (*waveFunction)(float);
};
