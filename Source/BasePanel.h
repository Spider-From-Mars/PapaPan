#pragma once

#include <JuceHeader.h>
#include "BaseColours.h"
#include "LFOComponent.h"
#include "MasterComponent.h"

//==============================================================================
/*
*/
class BasePanel  : public juce::Component
{
public:
    BasePanel(juce::AudioProcessorValueTreeState& apvts);
    ~BasePanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasePanel)
    
    juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(
                                                                           BinaryData::ComicNeueBold_ttf,
                                                                           BinaryData::ComicNeueBold_ttfSize
                                                                           );
    
    juce::Label titleLabel {"Title Label", "PAPA PAN"};
    juce::Label companyLabel {"Company Name", "Hamlet, inc."};
    
    LFOComponent lfoComponent;
    MasterComponent masterComponent;
    
    void setMainStyleLabel(juce::Label& label, float fontSize);
    
    const int roundedRectWidth = 205;
};
 
