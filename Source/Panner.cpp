#include "Panner.h"

void Modulation::updatePhaseState(float hertzRate,
                                  double duration,
                                  double sampleRate,
                                  const juce::AudioPlayHead::PositionInfo& posInfo)
{
    constexpr float twoPI = juce::MathConstants<float>::twoPi;
    
    switch (modType) {
        case Modulation::Modes::Hertz_Retrig:
        {
            setPhaseIncrement(twoPI * hertzRate / sampleRate);
            break;
        }
            
        case Modulation::Modes::Beat_Retrig:
        {
            auto bpm = posInfo.getBpm().orFallback(120.f);
            float beatDuration = (bpm / 60) * duration;
            hertzRate = 1.0 / beatDuration;
            
            setPhaseIncrement(twoPI * hertzRate / sampleRate);
            break;
        }
            
        case Modulation::Modes::Hertz_Synced:
        {
            const double panPeriodSeconds = 1.0 / hertzRate;
            const double currentTime = *posInfo.getTimeInSeconds();
            
            setPhase(twoPI * std::fmod(currentTime, panPeriodSeconds) / panPeriodSeconds);
            break;
        }
            
        case Modulation::Modes::Beat_Synced:
        {
            auto ppqPosition = posInfo.getPpqPosition().orFallback(0);
            auto cycleLength = duration * 4.0;
            
            setPhase(twoPI * std::fmod(ppqPosition, cycleLength) / cycleLength);
            break;
        }
    }
}

void Modulation::advance()
{
    phase += phaseIncrement;
    if (phase >= juce::MathConstants<float>::twoPi)
        phase -= juce::MathConstants<float>::twoPi;
}

void Modulation::setPhase(float newPhase)
{
    phase = std::fmod(newPhase, juce::MathConstants<float>::twoPi);
    
    if (phase < 0)
        phase += juce::MathConstants<float>::twoPi;
}

float Modulation::triangle(float phase)
{
    constexpr auto period = juce::MathConstants<float>::twoPi;
    
    phase = std::fmod(phase + period / 4.0f, period) / period;
    
    return 1 - 4.0f * std::abs(phase - 0.5f);
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
    
    mod.updatePhaseState(*apvts.getRawParameterValue("HERTZRATE"),
                         noteDurations[*apvts.getRawParameterValue("DURATION")],
                         sampleRate,
                         posInfo);
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
        
        if (mod.getModType() == Modulation::Modes::Beat_Retrig or
            mod.getModType() == Modulation::Modes::Hertz_Retrig)
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
