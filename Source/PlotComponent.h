#pragma once

#include <JuceHeader.h>
#include "Panner.h"

class PlotComponent : public juce::Component
{
public:
    PlotComponent();
    ~PlotComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlotComponent)
    
    void drawGrid(juce::Graphics& g, int x, int y, int height, int linesNum);
    void drawPlot(juce::Graphics& g, int xSize);
};
