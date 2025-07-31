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

class Modulation
{
public:
    void advance();
    void setPhaseIncrement(float increment) { phaseIncrement = increment; }
    float getPhase() const { return phase; }
    
    static float triangle(float phase);
    
private:
    float phase = 0.0;
    float phaseIncrement = 0.0;
};

class Panner {
public:
    
    enum class waveType
    {
        sin,
        triangle
    };
    
    Panner(Modulation& mod);
    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void update(int newMix, waveType newWave, const int newMode, const int newNoteDuration, float newHertzRate = 0, double newBpm = 0);
    
private:
    float mix = 1.0;
    waveType wave = waveType::sin;
    float hertzRate = 0.0;
    double sampleRate = 44100.0;
    
    std::array<double, 18> noteDurations = {
        32.0, 16.0, 8.0, 4.0,
        3.0, 2.0, 1.5, 1.0,
        3.0/4, 1.0/2, 3.0/8, 1.0/4, 3.0/16, 1.0/8, 1.0/16,
        1.0/32, 1.0/64, 1.0/128
    };
    
    Modulation &mod;
    
    float applyWave(waveType wave, float phase);
};
