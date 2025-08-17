#include "Panner.h"

void Modulation::advance()
{
    if (modType == Modes::Beat_Retrig or modType == Modes::Hertz_Retrig)
    {
        phase += phaseIncrement;
        if (phase >= juce::MathConstants<float>::twoPi)
            phase -= juce::MathConstants<float>::twoPi;
    }
    else if (modType == Modes::Beat_Synced)
    {
        
    }
}

float Modulation::triangle(float phase)
{
    constexpr auto period = juce::MathConstants<float>::twoPi;
    
    phase = std::fmod(phase + period / 4.0f, period) / period;
    
    return 1 - 4.0f * std::abs(phase - 0.5f);
}


Panner::Panner(Modulation& mod) : mod(mod)
{
}

void Panner::prepare(juce::dsp::ProcessSpec& spec)
{
    jassert (spec.sampleRate > 0);
    jassert (spec.numChannels > 0);

    sampleRate = spec.sampleRate;
}

void Panner::update(const juce::AudioProcessorValueTreeState& apvts, const juce::AudioPlayHead::PositionInfo& posInfo)
{
    mix = *apvts.getRawParameterValue("MIX") / 100.0;
    
    auto waveIndex = static_cast<int>(*apvts.getRawParameterValue("WAVETYPE"));
    wave = static_cast<Panner::waveType>(waveIndex);
    
    auto mode = static_cast<int>(*apvts.getRawParameterValue("MODE"));
    auto newMode = static_cast<Modulation::Modes>(mode);
    mod.setModType(newMode);
    
    switch (newMode) {
        case Modulation::Modes::Hertz_Retrig:
            hertzRate = *apvts.getRawParameterValue("HERTZRATE");
            break;
        case Modulation::Modes::Beat_Retrig:
        {
            auto bpm = posInfo.getBpm();
            auto newBpm = bpm ? *bpm : 120.f;

            float beatDuration = (newBpm / 60) * noteDurations[*apvts.getRawParameterValue("DURATION")];
            hertzRate = 1.0 / beatDuration;
            break;
        }
        case Modulation::Modes::Hertz_Synced:
            break;
        case Modulation::Modes::Beat_Synced:
            break;
    }
    
    mod.setPhaseIncrement(juce::MathConstants<float>::twoPi * hertzRate / sampleRate);
}

void Panner::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getWritePointer(1);
    
    constexpr float smoothingFactor = 0.05;
    constexpr float GAIN = juce::MathConstants<float>::sqrt2;
    
    for (int i = 0; i < numSamples; i++)
    {
        float dryLeft = left[i];
        float dryRight = right[i];
        
        float pan = 0.5 + 0.5 * applyWave(wave, mod.getPhase());
        
        mod.advance();
        
        float wetLeft = dryLeft * std::cos(pan * juce::MathConstants<float>::halfPi) * GAIN;
        float wetRight = dryRight * std::sin(pan * juce::MathConstants<float>::halfPi) * GAIN;
        
        
        float smoothGain = smoothingFactor * GAIN + (1.0 - smoothingFactor);
        
        left[i] = (dryLeft * (1-mix) + wetLeft * mix) * smoothGain;
        right[i] = (dryRight * (1-mix) + wetRight * mix) * smoothGain;
    }
}

float Panner::applyWave(waveType wave, float phase)
{
    float modValue = 0.0;
    
    switch (wave) {
        case waveType::sin:
            modValue = -std::sin(phase);
            break;
            
        case waveType::triangle:
            modValue = -Modulation::triangle(phase);
            break;
    }
    
    return modValue;
}
