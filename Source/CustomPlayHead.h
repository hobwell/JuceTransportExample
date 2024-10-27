/*
  ==============================================================================

	CustomAudioPlayHead.h
	Created: 16 Sep 2024 5:32:45pm
	Author:  Nicholas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TransportTree.h"
#include "APVTSWrapper.h"

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
	CustomPlayHead(juce::AudioProcessor& proc, double sampleRateIn, APVTSWrapper& wrapper);

	~CustomPlayHead() override;

	juce::Optional<PositionInfo> getPosition(int bufferSize) const;

private:
	APVTSWrapper& transportWrapper;

	juce::AudioProcessor& processor;
	juce::AudioPlayHead::PositionInfo& info = juce::AudioPlayHead::PositionInfo();
	juce::AudioPlayHead::PositionInfo& hostInfo = juce::AudioPlayHead::PositionInfo();

	// internal transport info - represents the current state of the playhead
	mutable double bpm = 120.0;
	mutable bool isPlaying = false;
	mutable double ppq = 0.0;
	mutable uint64_t bufferEnd = 0;
	mutable uint64_t bufferStart = 0;
	mutable double sampleRate = 48000.0;
	mutable uint64_t timeNs = 0;
	mutable juce::AudioPlayHead::TimeSignature timeSig{ 4, 4 };

	// external transport info - represents the "external" state of the transport (from either the host or the GUI)
	bool isStandalone = false;
	mutable float exTempo;
	mutable bool exPlaying;
	mutable juce::AudioPlayHead::TimeSignature exTimeSig{ 4, 4 };

	bool useThisPlayhead = false;

	// for calculating ppq (in the absence of host position info)
	mutable bool needsUpdate = false;
	mutable int bufferSize = 480;
	mutable float samplesPerBeat = sampleRate * secondsPerBeat;
	mutable float secondsPerBeat = 60.f / bpm; // 120 bpm
	mutable float beatsPerQuarterNote = timeSig.denominator / 4.f;
	mutable float quarterNotesPerBuffer = bufferSize / (samplesPerBeat * beatsPerQuarterNote);

	juce::Optional<PositionInfo> getPosition() const override;

	void updatePosition() const;

	void recalculate() const;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomPlayHead);
};