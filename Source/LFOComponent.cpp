#include <JuceHeader.h>
#include "LFOComponent.h"

//==============================================================================
LFOComponent::LFOComponent()
{

}

LFOComponent::~LFOComponent()
{
}

void LFOComponent::paint (juce::Graphics& g)
{
    ControlComponent::paint(g);
    setRotatedLabel(g, "LFO");
}

void LFOComponent::resized()
{
    
}
