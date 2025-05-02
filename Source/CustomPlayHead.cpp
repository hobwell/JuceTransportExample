/*
  ==============================================================================

    CustomAudioPlayHead.cpp
    Created: 16 Sep 2024 5:32:45pm
    Author:  Nicholas

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CustomPlayHead.h"

CustomPlayHead::CustomPlayHead(juce::AudioProcessor& proc, double sampleRateIn, ApvtsWrapper& wrapper)
    : processor(proc),
    transportWrapper(wrapper),
    sampleRate(sampleRateIn)
{
    // initialize all the transport parameters (to get them into the tree)
    transportWrapper.bar_length = 4;
    transportWrapper.beat_duration = 4;
    transportWrapper.host_controls_playing = false;
    transportWrapper.host_controls_position = false;
    transportWrapper.host_controls_tempo = false;
    transportWrapper.host_controls_tempo_speed = false;
    transportWrapper.host_controls_time_signature = false;
    transportWrapper.rewind_flag = false;
    transportWrapper.playing = false;
    transportWrapper.rewind_flag = false;
    transportWrapper.sample_rate = sampleRate;
    transportWrapper.tempo = 120.f;
    transportWrapper.tempo_speed = 0.25f; // 1/4 note duration
    transportWrapper.time_sig_controls_tempo_speed = false;

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
    if (isPlaying)
    {
        if (transportWrapper.host_controls_playing && processor.getPlayHead()->getPosition().hasValue())
        {
            hostInfo = *processor.getPlayHead()->getPosition();

            // if the host provides a value, use it, otherwise fall back to the internal value
            if (transportWrapper.host_controls_playing && hostInfo.getHostTimeNs().hasValue())
            {
                timeNs = *hostInfo.getHostTimeNs();
            }
            else
            {
                timeNs += bufferSize * 1e9 / sampleRate;
            }

            if (transportWrapper.host_controls_tempo && hostInfo.getBpm().hasValue())
            {
                tempo = *hostInfo.getBpm();
            }

            if (transportWrapper.host_controls_position && hostInfo.getPpqPosition().hasValue())
            {
                ppq = *hostInfo.getPpqPosition();
            }
            else
            {
                ppq += bufferSize / (samplesPerBeat * beatsPerQuarterNote);
            }

            if (transportWrapper.host_controls_position && hostInfo.getTimeInSamples().hasValue())
            {
                bufferStart = *hostInfo.getTimeInSamples();
            }
            bufferEnd = bufferStart + bufferSize;
        }
        else
        {
            // advance the position based on the buffer size
            bufferEnd = bufferStart + bufferSize;
            timeNs += bufferSize * 1e9 / sampleRate;
            quarterNotesPerBuffer = bufferSize / (samplesPerBeat * beatsPerQuarterNote);
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

    return getPosition();
}

/*
* Recalculate time domain values
*/
void CustomPlayHead::recalculate() const
{
    beatsPerQuarterNote = 0.25f / tempoSpeed;
    secondsPerBeat = 60.f / tempo;
    samplesPerBeat = sampleRate * secondsPerBeat;
    needsUpdate = false;
}

/*
* Synchronize the playhead state, ahead of calling getPosition()
*/
void CustomPlayHead::synchronizeState()
{
    if (transportWrapper.rewind_flag)
    {
        bufferEnd = 0;
        bufferStart = 0;
        timeNs = 0;
        ppq = 0;
        transportWrapper.rewind_flag = false;
        updatePosition();
    }

    hostInfo = *processor.getPlayHead()->getPosition();

    // assumption: all hosts (outside standalone) will provide at least the playing state
    if (isStandalone)
    {
        nextPlaying = transportWrapper.playing;
        transportWrapper.host_controls_playing = false;
    }
    else
    {
        nextPlaying = hostInfo.getIsPlaying();
        transportWrapper.host_controls_playing = true;
    }

    // check if the playing state has changed
    if (isPlaying != nextPlaying)
    {
        isPlaying = !isPlaying;
        // set the playing state in the transport params
        //updateParameter(IDS::playing, isPlaying);
        transportWrapper.playing = isPlaying;
        DBG("TParam: " << *transportWrapper.apvts.getRawParameterValue(IDS::playing));
    }

    // if the host provides a tempo, use it
    if (hostInfo.getBpm().hasValue())
    {
        nextTempo = *hostInfo.getBpm();
        transportWrapper.host_controls_tempo = true;
    }
    else
    {
        nextTempo = transportWrapper.apvts.getRawParameterValue(IDS::tempo)->load();
        transportWrapper.host_controls_tempo = false;
    }

    // check if the tempo has changed
    if (tempo != nextTempo)
    {
        tempo = nextTempo;
        needsUpdate = true;
        if (transportWrapper.host_controls_tempo)
        {
            // set the tempo in the transport params
            transportWrapper.updateParameter(IDS::tempo, nextTempo);
        }
    };

    // check if the host has a time signature
    if (hostInfo.getTimeSignature().hasValue())
    {
        nextTimeSig = *hostInfo.getTimeSignature();
        transportWrapper.host_controls_time_signature = true;
        //updateParameter(IDS::host_controls_time_sig, true);
        // TODO - need to know if all DAWs do this:
        //updateParameter(IDS::host_controls_tempo_speed, true);
        transportWrapper.host_controls_tempo_speed = true;
        //updateParameter(IDS::time_sig_controls_tempo_speed, true);
        transportWrapper.time_sig_controls_tempo_speed = true;
    }
    else
    {
        nextTimeSig.numerator = transportWrapper.bar_length;
        nextTimeSig.denominator = transportWrapper.beat_duration;
        //updateParameter(IDS::host_controls_time_sig, false);
        transportWrapper.host_controls_time_signature = false;
        transportWrapper.updateParameter(IDS::host_controls_tempo_speed, false);
        transportWrapper.host_controls_tempo_speed = false;
        transportWrapper.updateParameter(IDS::time_sig_controls_tempo_speed, false);
        transportWrapper.time_sig_controls_tempo_speed = false;
    }

    // check if the time signature has changed
    if (timeSig.numerator != nextTimeSig.numerator || timeSig.denominator != nextTimeSig.denominator)
    {
        timeSig.numerator = nextTimeSig.numerator;
        timeSig.denominator = nextTimeSig.denominator;
        needsUpdate = true;

        // set the time signature in the transport params
        transportWrapper.updateParameter(IDS::bar_length, nextTimeSig.numerator);
        transportWrapper.updateParameter(IDS::beat_duration, nextTimeSig.denominator);
        DBG("Time Signature: " << timeSig.numerator << "/" << timeSig.denominator);
    }

    // check if tempo relative note duration has changed
    if (transportWrapper.time_sig_controls_tempo_speed)
    {
        // set the tempo relative note duration based on the time signature
        nextTempoSpeed = 1.f / timeSig.denominator;
    }
    else
    {
        nextTempoSpeed = transportWrapper.tempo_speed;
    }

    if (tempoSpeed != nextTempoSpeed)
    {
        tempoSpeed = nextTempoSpeed;
        needsUpdate = true;

        // set the tempo relative note duration in the transport params
        transportWrapper.updateParameter(IDS::tempo_speed, nextTempoSpeed);
    }

    // check if the host has position info (but only if the host has play control)
    if (transportWrapper.host_controls_playing && (hostInfo.getPpqPosition().hasValue() || hostInfo.getTimeInSamples().hasValue() || hostInfo.getHostTimeNs().hasValue() || hostInfo.getTimeInSeconds().hasValue()))
    {
        transportWrapper.host_controls_position = true;
    }
    else
    {
        transportWrapper.host_controls_position = false;
    }

    if (needsUpdate)
    {
        recalculate();
    }
}

/*
* Set position info
*/
void CustomPlayHead::updatePosition() const
{
    info.setHostTimeNs(timeNs);
    info.setBpm(tempo);
    info.setTimeInSamples(bufferStart);
    info.setTimeInSeconds(bufferStart / sampleRate);
    info.setPpqPosition(ppq);

    // after updating the position, advance the buffer start
    bufferStart = bufferEnd;

    // report the ppq position
    transportWrapper.setPpq(ppq); // not using a cached value for ppq as it triggers a listener chain which can cause concurrent access errors on the listener list
    transportWrapper.setPos(ppq);
}