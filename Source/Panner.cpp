#include "Panner.h"

void Modulation::advance()
{
    phase += phaseIncrement;
    if (phase >= twoPI)
        phase -= twoPI;
}

void Modulation::hertzRetrigProcess(float hertzRate)
{
    phaseIncrement = twoPI * hertzRate / sampleRate;
}

void Modulation::beatRetrigProcess(double duration, double bpm)
{
    float beatDuration = (bpm / 60) * duration;
    float hertzRate = 1.f / beatDuration;

    phaseIncrement = twoPI * hertzRate / sampleRate;
}

void Modulation::hertzSyncedProcess(float hertzRate,
                                    const juce::AudioPlayHead::PositionInfo &posInfo)
{
    if (needResetPhase(posInfo))
    {
        auto currentTime = posInfo.getTimeInSeconds().orFallback(0);
        auto periodInSeconds = 1.f / hertzRate;

        setPhase(twoPI * std::fmod(currentTime, periodInSeconds) / periodInSeconds);
    }

    phaseIncrement = twoPI * hertzRate / sampleRate;
}

void Modulation::beatSyncedProcess(double duration,
                                   const juce::AudioPlayHead::PositionInfo &posInfo)
{
    if (needResetPhase(posInfo))
    {
        auto ppqPosition = posInfo.getPpqPosition().orFallback(0);
        auto cycleLength = duration * 4.f;

        setPhase(twoPI * std::fmod(ppqPosition, cycleLength) / cycleLength);
    }

    float beatDuration = (posInfo.getBpm().orFallback(120.f) / 60) * duration;
    float hertzRate = 1.f / beatDuration;

    phaseIncrement = twoPI * hertzRate / sampleRate;
}

void Modulation::setPhase(float newPhase)
{
    phase = std::fmod(newPhase, twoPI);

    if (phase < 0)
        phase += twoPI;
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

float Modulation::triangle(float phase)
{
    phase = std::fmod(phase + twoPI / 4.f, twoPI) / twoPI;

    return 1 - 4.f * std::abs(phase - 0.5);
}

/* Panner
=================================================*/

void Panner::prepare(juce::dsp::ProcessSpec &spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;
    mod.prepare(spec);
}

void Panner::update(const juce::AudioProcessorValueTreeState &apvts,
                    const juce::AudioPlayHead::PositionInfo &posInfo, float f0)
{
    mix = *apvts.getRawParameterValue("MIX") / 100.f;

    auto waveIndex = static_cast<int>(*apvts.getRawParameterValue("WAVETYPE"));
    wave = static_cast<Panner::waveType>(waveIndex);

    auto mode = static_cast<int>(*apvts.getRawParameterValue("MODE"));
    mod.setModType(static_cast<Modulation::Modes>(mode));

    using Modes = Modulation::Modes;
    switch (mod.getModType())
    {
    case Modes::Hertz_Retrig:
        mod.hertzRetrigProcess(*apvts.getRawParameterValue("HERTZRATE"));
        break;

    case Modes::Beat_Retrig:
        mod.beatRetrigProcess(noteDurations[*apvts.getRawParameterValue("DURATION")],
                              posInfo.getBpm().orFallback(120.f));
        break;

    case Modes::Hertz_Synced:
        mod.hertzSyncedProcess(*apvts.getRawParameterValue("HERTZRATE"), posInfo);
        break;

    case Modes::Beat_Synced:
        mod.beatSyncedProcess(noteDurations[*apvts.getRawParameterValue("DURATION")], posInfo);
        break;

    case Modes::Pitch_To_Rate:
        mod.hertzRetrigProcess(f0);
        break;
    }
}

void Panner::process(juce::AudioBuffer<float> &buffer)
{
    auto *left = buffer.getWritePointer(0);
    auto *right = buffer.getWritePointer(1);

    static float GAIN = juce::MathConstants<float>::sqrt2;
    static float smoothingFactor = 0.05;
    static float smoothGain = smoothingFactor * GAIN + (1.f - smoothingFactor);

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float dryLeft = left[i];
        float dryRight = right[i];

        float pan = 0.5 + 0.5 * applyWave(wave, mod.getPhase());

        mod.advance();

        float wetLeft = dryLeft * std::cos(pan * juce::MathConstants<float>::halfPi) * GAIN;
        float wetRight = dryRight * std::sin(pan * juce::MathConstants<float>::halfPi) * GAIN;

        left[i] = (dryLeft * (1 - mix) + wetLeft * mix) * smoothGain;
        right[i] = (dryRight * (1 - mix) + wetRight * mix) * smoothGain;
    }
}

float Panner::applyWave(waveType wave, float phase)
{
    float modValue = 0;

    switch (wave)
    {
    case waveType::sin:
        modValue = -std::sin(phase);
        break;

    case waveType::triangle:
        modValue = -Modulation::triangle(phase);
        break;
    }

    return modValue;
}
