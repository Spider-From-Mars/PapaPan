#include "Panner.h"

void Modulation::updatePhaseState(float hertzRate,
                                  double duration,
                                  double sampleRate,
                                  const juce::AudioPlayHead::PositionInfo& posInfo)
{
    switch (modType) {
        case Modulation::Modes::Hertz_Retrig:
        {
            hertzRetrigProcess(hertzRate, sampleRate);
            break;
        }
            
        case Modulation::Modes::Beat_Retrig:
        {
            beatRetrigProcess(duration, posInfo.getBpm().orFallback(120.f), sampleRate);
            break;
        }
            
        case Modulation::Modes::Hertz_Synced:
        {
            hertzSyncedProcess(hertzRate, sampleRate, posInfo);
            break;
        }
            
        case Modulation::Modes::Beat_Synced:
        {
            beatSyncedProcess(duration, sampleRate, posInfo);
            break;
        }
    }
}

bool Modulation::needResetPhase(const juce::AudioPlayHead::PositionInfo &posInfo)
{
    bool resetPhase = false;
    auto ppqPosition = posInfo.getPpqPosition().orFallback(0);
    
    if (posInfo.getIsPlaying() != lastPosInfo.wasPlaying)
    {
        if (posInfo.getIsPlaying())
        {
            resetPhase = true;
        }
        lastPosInfo.wasPlaying = posInfo.getIsPlaying();
    }
    
    else if (posInfo.getIsPlaying())
    {
        if (std::abs(ppqPosition - lastPosInfo.ppqPosition) > 0.05)
        {
            resetPhase = true;
        }
        lastPosInfo.ppqPosition = ppqPosition;
    }
    
    return resetPhase;
}

void Modulation::hertzRetrigProcess(float hertzRate, double sampleRate)
{
    setPhaseIncrement(twoPI * hertzRate / sampleRate);
}

void Modulation::beatRetrigProcess(double duration, double bpm, double sampleRate)
{
    float beatDuration = (bpm / 60) * duration;
    auto hertzRate = 1.0 / beatDuration;
    
    setPhaseIncrement(twoPI * hertzRate / sampleRate);
}

void Modulation::hertzSyncedProcess(float hertzRate,
                                    double sampleRate,
                                    const juce::AudioPlayHead::PositionInfo &posInfo)
{
    if (needResetPhase(posInfo))
    {
        auto currentTime = posInfo.getTimeInSeconds().orFallback(0);
        auto periodInSeconds = 1.0 / hertzRate;
        
        setPhase(twoPI * std::fmod(currentTime, periodInSeconds) / periodInSeconds);
    }
    
    setPhaseIncrement(twoPI * hertzRate / sampleRate);
}

void Modulation::beatSyncedProcess(double duration,
                                   double sampleRate,
                                   const juce::AudioPlayHead::PositionInfo &posInfo)
{
    if (needResetPhase(posInfo))
    {
        auto ppqPosition = posInfo.getPpqPosition().orFallback(0);
        auto cycleLength = duration * 4.0;
        
        setPhase(twoPI * std::fmod(ppqPosition, cycleLength) / cycleLength);
    }
    
    float beatDuration = (posInfo.getBpm().orFallback(120.f) / 60) * duration;
    auto hertzRate = 1.0 / beatDuration;
    
    setPhaseIncrement(twoPI * hertzRate / sampleRate);
}

void Modulation::advance()
{
    phase += phaseIncrement;
    if (phase >= twoPI)
        phase -= twoPI;
}

void Modulation::setPhase(float newPhase)
{
    phase = std::fmod(newPhase, twoPI);
    
    if (phase < 0)
        phase += twoPI;
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
