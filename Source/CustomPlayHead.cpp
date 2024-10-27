/*
  ==============================================================================

	CustomAudioPlayHead.cpp
	Created: 16 Sep 2024 5:32:45pm
	Author:  Nicholas

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CustomPlayHead.h"

CustomPlayHead::CustomPlayHead(juce::AudioProcessor& proc, double sampleRateIn, APVTSWrapper& wrapper)
	: processor(proc),
	transportWrapper(wrapper),
	sampleRate(sampleRateIn)
{
	transportWrapper.sample_rate = sampleRate;

	// check if the host is a standalone app
	isStandalone = juce::JUCEApplicationBase::isStandaloneApp();
	recalculate();
	updatePosition();
}

CustomPlayHead::~CustomPlayHead() {}

/*
*  Calculate the current position of the playhead based on the buffer size and the host transport information
*/
juce::Optional<juce::AudioPlayHead::PositionInfo> CustomPlayHead::getPosition() const
{
	if (isPlaying) {
		if (transportWrapper.host_controls_playing && processor.getPlayHead()->getPosition().hasValue()) {
			hostInfo = *processor.getPlayHead()->getPosition();

			// if the host provides a value, use it, otherwise fall back to the internal value
			if (transportWrapper.host_controls_playing && hostInfo.getHostTimeNs().hasValue()) {
				timeNs = *hostInfo.getHostTimeNs();
			}
			else {
				timeNs = bufferStart * 1e9 / sampleRate; // TODO - update this to accumulate time, rather than calcaulating it from the buffer start
			}

			if (transportWrapper.host_controls_tempo && hostInfo.getBpm().hasValue()) {
				bpm = *hostInfo.getBpm();
			}

			if (transportWrapper.host_controls_position && hostInfo.getPpqPosition().hasValue()) {
				ppq = *hostInfo.getPpqPosition();
			}
			else {
				ppq = bufferStart / (samplesPerBeat * beatsPerQuarterNote); // TODO - update this to accumulate time, rather than calcaulating it from the buffer start
			}

			if (transportWrapper.host_controls_position && hostInfo.getTimeInSamples().hasValue()) {
				bufferStart = *hostInfo.getTimeInSamples();
			}
			bufferEnd = bufferStart + bufferSize;
		}
		else {
			// advance the position based on the buffer size
			quarterNotesPerBuffer = bufferSize / (samplesPerBeat * beatsPerQuarterNote);
			bufferEnd = bufferStart + bufferSize;
			timeNs += bufferStart * 1e9 / sampleRate;
			//ppq = bufferStart / (samplesPerBeat * beatsPerQuarterNote); // TODO - update this to accumulate time, rather than calcaulating it from the buffer start
			ppq += quarterNotesPerBuffer;
		}
		updatePosition();
	}

	return info;
}

/*
*  Synchronizes the playhead transport information with the host transport information then returns the current position info
*/
juce::Optional<juce::AudioPlayHead::PositionInfo> CustomPlayHead::getPosition(int bufferSize) const
{
	this->bufferSize = bufferSize;

	if (transportWrapper.rewind_flag) {
		bufferEnd = 0;
		bufferStart = 0;
		timeNs = 0;
		ppq = 0;
		transportWrapper.rewind_flag = false;
	}

	//if (processor.getPlayHead()->getPosition().hasValue()) { // this always seems to be true, so skip it for now
	hostInfo = *processor.getPlayHead()->getPosition();

	// assumption: all hosts (outside standalone) will provide at least the playing state
	if (isStandalone) {
		exPlaying = transportWrapper.playing;
		transportWrapper.host_controls_playing = false;
	}
	else {
		exPlaying = hostInfo.getIsPlaying();
		transportWrapper.host_controls_playing = true;
	}

	// check if the playing state has changed
	if (isPlaying != exPlaying) {
		isPlaying = !isPlaying;
		// set the playing state in the transport params
		transportWrapper.playing = isPlaying;
		DBG("TParam: " << *transportWrapper.apvts.getRawParameterValue(IDS::playing));
	}

	// if the host provides a tempo, use it
	if (hostInfo.getBpm().hasValue()) {
		exTempo = *hostInfo.getBpm();
		transportWrapper.host_controls_tempo = true;
	}
	else {
		exTempo = transportWrapper.tempo;
		transportWrapper.host_controls_tempo = false;
	}

	// check if the tempo has changed
	if (bpm != exTempo) {
		bpm = exTempo;
		needsUpdate = true;
		if (transportWrapper.host_controls_tempo) {
			// set the tempo in the transport params
			transportWrapper.tempo = exTempo;
		}
	};

	// check if the host has a time signature
	if (hostInfo.getTimeSignature().hasValue()) {
		exTimeSig = *hostInfo.getTimeSignature();
		transportWrapper.host_controls_time_signature = true;
	}
	else {
		exTimeSig.numerator = transportWrapper.bar_length;
		exTimeSig.denominator = transportWrapper.beat_duration;
		transportWrapper.host_controls_time_signature = false;
	}

	// check if the time signature has changed
	if (timeSig.numerator != exTimeSig.numerator || timeSig.denominator != exTimeSig.denominator) {
		timeSig.numerator = exTimeSig.numerator;
		timeSig.denominator = exTimeSig.denominator;
		needsUpdate = true;
		// set the time signature in the transport params
		transportWrapper.bar_length = exTimeSig.numerator;
		transportWrapper.beat_duration = exTimeSig.denominator;
		DBG("Time Signature: " << timeSig.numerator << "/" << timeSig.denominator);
	}

	// check if the host has position info (but only if the host has play control)
	if (transportWrapper.host_controls_playing && (hostInfo.getPpqPosition().hasValue() || hostInfo.getTimeInSamples().hasValue() || hostInfo.getHostTimeNs().hasValue() || hostInfo.getTimeInSeconds().hasValue())) {
		transportWrapper.host_controls_position = true;
	}
	else {
		transportWrapper.host_controls_position = false;
	}

	if (needsUpdate) {
		recalculate();
	}
	//}

	return getPosition();
}

/*
* Recalculate time domain values
*/
void CustomPlayHead::recalculate() const
{
	beatsPerQuarterNote = timeSig.denominator / 4.f;
	secondsPerBeat = 60.f / bpm;
	samplesPerBeat = sampleRate * secondsPerBeat;
	needsUpdate = false;
}

/*
* Set position info
*/
void CustomPlayHead::updatePosition() const
{
	info.setHostTimeNs(timeNs);
	info.setBpm(bpm);
	info.setTimeInSamples(bufferStart);
	info.setTimeInSeconds(bufferStart / sampleRate);
	info.setPpqPosition(ppq);

	// after updating the position, advance the buffer start
	bufferStart = bufferEnd;

	// report the ppq position
	transportWrapper.ppq = ppq; // BUG: this assignment is causing the listener to trigger, which is sometimes causing an error
}