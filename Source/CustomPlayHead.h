/*
  ==============================================================================

    CustomAudioPlayHead.h
    Created: 16 Sep 2024 5:32:45pm
    Author:  Nicholas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TransportParameters.h"

/// <summary>
/// A custom playhed that will sync changes to transport information into an 
/// APVTS, whether the plugin is running in standalone mode or within a host.
/// 
/// When running in a host, this is primarily needed to sync changes from 
/// the host to the APVTS.
/// 
/// When running in standalone mode, this is primarily needed used to pull the 
/// current transport values from the APVTS that were updated by the UI.
/// </summary>
class CustomPlayHead : public juce::AudioPlayHead
{
public:
    CustomPlayHead(juce::AudioProcessor& proc, double sampleRateIn, TransportParameters& wrapper);

    ~CustomPlayHead() override;

    juce::Optional<PositionInfo> getPosition(int bufferSize) const;

    void synchronizeState();
private:
    TransportParameters& transportParams;

    juce::AudioProcessor& processor;
    juce::AudioPlayHead::PositionInfo& info = juce::AudioPlayHead::PositionInfo();
    juce::AudioPlayHead::PositionInfo& hostInfo = juce::AudioPlayHead::PositionInfo();

    // internal transport info - represents the current state of the playhead
    bool isPlaying = false;
    mutable double ppq = 0.0;
    mutable uint64_t bufferPos = 0;
    double sampleRate = 48000.0;
    mutable double tempo = 120.0;
    float tempoSpeed = 0.25f; // what fraction to use for the tempo relative note duration (0.25 = quarter note, 0.5 = half note, etc.)
    mutable uint64_t timeNs = 0;
    juce::AudioPlayHead::TimeSignature timeSig {4, 4};

    // external transport info - represents the "external" state of the transport (from either the host or the GUI)
    bool isStandalone = false;
    float nextTempo;
    float nextTempoSpeed;
    bool nextPlaying;
    juce::AudioPlayHead::TimeSignature nextTimeSig {4, 4};

    bool useThisPlayhead = false;

    // for calculating ppq (in the absence of host position info)
    mutable bool needsUpdate = false;
    mutable int bufferSize = 480;
    mutable float samplesPerBeat = sampleRate * secondsPerBeat;
    mutable float secondsPerBeat = 60.f / tempo; // 120 tempo
    mutable float beatsPerQuarterNote = (1.f / timeSig.denominator) / tempoSpeed;
    mutable float quarterNotesPerBuffer = bufferSize / (samplesPerBeat * beatsPerQuarterNote);

    juce::Optional<PositionInfo> getPosition() const override;

    void advancePlayHead() const;
    void changePosition() const;
    void updatePosition() const;

    void recalculate() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomPlayHead);
};