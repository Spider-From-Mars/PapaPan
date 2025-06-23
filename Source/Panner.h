#pragma once
#include <JuceHeader.h>

enum Modes
{
    Hertz_Synced,
    Beat_Synced
};

enum NoteDuration
{
    Half,
    Quarter,
    Eighth,
    Sixteenth
};

class Panner {
public:
    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void update(int newMix, int newWaveType, const int newMode, const int newNoteDuration, float newHertzRate = 0, double newBpm = 0);
    
private:
    float mix = 1.0;
    int waveType = 0;
    float hertzRate = 0.0;
    float phase = 0.0;
    float phaseIncrement = 0.0;
    double sampleRate = 44100.0;
    
    std::array<double, 11> noteDurations = {1.0/2, 1.0/4, 1.0/8, 1.0/16, 1.0/32, 1.0/64, 4.0/4, 6.0/4, 8.0/4, 12.0/4, 16.0/4};
    
    float applyWave(int waveType, float phase);
};
