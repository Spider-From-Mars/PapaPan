#pragma once
#include <JuceHeader.h>

#include "RingBuffer.h"
#include "YinFFT.h"

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
    
    enum class Modes
    {
        Hertz_Retrig,
        Beat_Retrig,
        Hertz_Synced,
        Beat_Synced,
        Pitch_To_Rate
    };
    
    void advance();
    
    void hertzRetrigProcess(float hertzRate, double sampleRate);
    void beatRetrigProcess(double duration, double bpm, double sampleRate);
    void hertzSyncedProcess(float hertzRate, double sampleRate, const juce::AudioPlayHead::PositionInfo &posInfo);
    void beatSyncedProcess(double duration, double sampleRate, const juce::AudioPlayHead::PositionInfo &posInfo);
    
    void setPhaseIncrement(float increment) { phaseIncrement = increment; }
    void setPhase(float phase);
    void setModType(Modes mode) { modType = mode; }
    
    float getPhase() const { return phase; }
    Modes getModType() const { return modType; }
    
    
    static float triangle(float phase);
    
private:
    float phase = 0.0;
    float phaseIncrement = 0.0;
    const float twoPI = juce::MathConstants<float>::twoPi;
    Modes modType;
    
    struct
    {
        bool wasPlaying = false;
        double ppqPosition = 0;
    } lastPosInfo;
    
    bool needResetPhase(const juce::AudioPlayHead::PositionInfo& posInfo);
};

class Panner {
public:
    
    enum class waveType
    {
        sin,
        triangle
    };
    
    void prepare(juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void update(const juce::AudioProcessorValueTreeState& apvts,
                const juce::AudioPlayHead::PositionInfo& posInfo,
                float f0);
    
    const Modulation& getModulation() const { return mod; }
    
private:
    float mix = 1.0;
    waveType wave = waveType::sin;
    double sampleRate = 44100.0;
    
    std::array<double, 18> noteDurations = {
        32.0, 16.0, 8.0, 4.0,
        3.0, 2.0, 1.5, 1.0,
        3.0/4, 1.0/2, 3.0/8, 1.0/4, 3.0/16, 1.0/8, 1.0/16,
        1.0/32, 1.0/64, 1.0/128
    };
    
    Modulation mod;
    
    
    float applyWave(waveType wave, float phase);
};
