#include "Panner.h"

void Panner::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert (spec.sampleRate > 0);
    jassert (spec.numChannels > 0);

    sampleRate = spec.sampleRate;
}

void Panner::update(int newMix, int newWaveType, const int newMode, const int newNoteDuration, float newHertzRate, double newBpm)
{
    mix = newMix / 100.0;
    waveType = newWaveType;
    
    if (newMode == Modes::Hertz_Synced)
        hertzRate = newHertzRate;
    else
    {
        float beatDuration = (newBpm / 60) * noteDurations[newNoteDuration];
        hertzRate = 1.0 / beatDuration;
    }
    
    phaseIncrement = juce::MathConstants<float>::twoPi * hertzRate / sampleRate;
}

void Panner::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getWritePointer(1);
    
    const float smoothingFactor = 0.05;
    const float GAIN = juce::MathConstants<float>::sqrt2;
    
    for (int i = 0; i < numSamples; i++)
    {
        float dryLeft = left[i];
        float dryRight = right[i];
        
        float pan = (phaseIncrement == 0) ? 0.5 : (0.5 + 0.5 * applyWave(waveType, phase));
        
        phase += phaseIncrement;
        if (phase >= juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;
        
        float wetLeft = dryLeft * std::cos(pan * juce::MathConstants<float>::halfPi) * GAIN;
        float wetRight = dryRight * std::sin(pan * juce::MathConstants<float>::halfPi) * GAIN;
        
        
        float smoothGain = smoothingFactor * GAIN + (1.0 - smoothingFactor);
        
        left[i] = (dryLeft * (1-mix) + wetLeft * mix) * smoothGain;
        right[i] = (dryRight * (1-mix) + wetRight * mix) * smoothGain;
    }
}

float Panner::applyWave(int waveType, float phase)
{
    float modValue = 0.0;
    
    switch (waveType) {
        case 0:
            modValue = std::sin(phase);
            break;
            
        case 1:
            modValue = 2.0 * std::abs(
                phase / juce::MathConstants<float>::twoPi - std::floor(phase / juce::MathConstants<float>::twoPi + 0.5
            ));
            modValue = modValue * 2.0 - 1.0;
            modValue = std::tanh(2.0 * modValue);
            break;
    }
    
    return modValue;
}
