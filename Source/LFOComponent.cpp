#include <JuceHeader.h>
#include "LFOComponent.h"

//==============================================================================
LFOComponent::LFOComponent(juce::AudioProcessorValueTreeState& apvts) :
        lfoSlider(*apvts.getParameter("HERTZRATE"), "HZ")
{
    juce::AudioParameterChoice* mode = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("MODE"));
    modeSelector.addItemList(mode->choices, 2);
    modeSelector.addSeparator();
    
    juce::AudioParameterChoice* duration = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("DURATION"));
    juce::StringArray durations = duration->choices;
    for (int i = 0; i < durations.size(); i++) {
        durationSelector.addItem(durations[i], i+1);
        switch (i) {
            case 3:
            case 7:
            case 14:
            case 17:
                durationSelector.addSeparator();
        }
    }
    
    lfoSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "HERTZRATE", lfoSlider);
    modeSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts,
                                                                                                      "MODE",
                                                                                                      modeSelector);
    durationSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts,
                                                                                                          "DURATION",
                                                                                                          durationSelector);
    
    addAndMakeVisible(lfoSlider);
    addAndMakeVisible(modeSelector);
    addAndMakeVisible(durationSelector);
}

LFOComponent::~LFOComponent()
{
}

void LFOComponent::paint(juce::Graphics& g)
{
    ControlComponent::paint(g);
    setRotatedLabel(g, "LFO");
}

void LFOComponent::resized()
{
    auto bounds = getLocalBounds();
    auto activeArea = bounds.withTrimmedRight(getrotatedLabelHeight());
    const float sliderWidth = activeArea.getWidth() / 2 - 10;
    
    lfoSlider.setBounds(activeArea.removeFromRight(sliderWidth));
    
    activeArea.setHeight(20);
    activeArea.setCentre(activeArea.getWidth() / 2, bounds.getCentreY() - 15);
    modeSelector.setBounds(activeArea);
    
    float durationSelectorWidth = modeSelector.getWidth() / 2 + 5;
    durationSelector.setBounds(
                               modeSelector.getX() + durationSelectorWidth / 2,
                               modeSelector.getBottom() + 10,
                               durationSelectorWidth,
                               modeSelector.getHeight()
                            );
}
