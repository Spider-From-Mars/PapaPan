#pragma once

#include <JuceHeader.h>
#include "ControlComponent.h"

//==============================================================================
/*
*/
class MidiComponent  : public ControlComponent
{
public:
    MidiComponent();
    ~MidiComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiComponent)
};
