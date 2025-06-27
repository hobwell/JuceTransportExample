/*
  ==============================================================================

    CustomAudioPlayHead.cpp
    Created: 16 Sep 2024 5:32:45pm
    Author:  Nicholas

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CustomPlayHead.h"

CustomPlayHead::CustomPlayHead(juce::AudioProcessor& proc, double sampleRateIn, TransportParameters& wrapper)
    : processor(proc),
    transportParams(wrapper),
    sampleRate(sampleRateIn)
{
    // initialize all the transport parameters (to get them into the tree)
    *transportParams.bar_length = 4;
    *transportParams.beat_duration = 4;
    *transportParams.host_controls_playing = false;
    *transportParams.host_controls_position = false;
    *transportParams.host_controls_tempo = false;
    *transportParams.host_controls_tempo_speed = false;
    *transportParams.host_controls_time_signature = false;
    *transportParams.playing = false;
    *transportParams.reposition_flag = false;
    *transportParams.sample_rate = sampleRate;
    *transportParams.tempo = 120.f;
    *transportParams.tempo_speed = 0.25f; // 1/4 note duration
    *transportParams.time_sig_controls_tempo_speed = false;

    // check if the host is a standalone app
    isStandalone = juce::JUCEApplicationBase::isStandaloneApp();
    recalculate(transportParams);
    updatePosition();
}

CustomPlayHead::~CustomPlayHead() {}

/*
* change position based on incoming ppq
*/
void CustomPlayHead::changePosition() const
{
    // get the current ppq position according to the transport
    ppq = transportParams.getPpq();

    // calculate the current buffer position
    bufferPos = ppq * samplesPerBeat * beatsPerQuarterNote;

    // calculate the current time position
    timeNs = (bufferPos / sampleRate) * 1e9;

    // clear the reposition flag
    *transportParams.reposition_flag = false;
    updatePosition();
}

/*
*  Calculate the current position of the playhead based on the buffer size and the host transport information
*/
juce::Optional<juce::AudioPlayHead::PositionInfo> CustomPlayHead::getPosition() const
{
    if (isPlaying)
    {
        if (*transportParams.host_controls_playing && processor.getPlayHead()->getPosition().hasValue())
        {
            hostInfo = *processor.getPlayHead()->getPosition();

            if (*transportParams.host_controls_position)
            {
                if (hostInfo.getTimeInSamples().hasValue())
                {
                    bufferPos = *hostInfo.getTimeInSamples();
                }

                if (hostInfo.getHostTimeNs().hasValue())
                {
                    timeNs = *hostInfo.getHostTimeNs();
                }
            
                if (hostInfo.getPpqPosition().hasValue())
                {
                    ppq = *hostInfo.getPpqPosition();
                }
            }

            if (*transportParams.host_controls_tempo && hostInfo.getBpm().hasValue())
            {
                tempo = *hostInfo.getBpm();
            }
        }
        updatePosition();
        advancePlayHead();
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
void CustomPlayHead::recalculate(TransportParameters& params) const
{
    beatsPerQuarterNote = 0.25f / tempoSpeed;
    secondsPerBeat = 60.f / tempo;
    samplesPerBeat = sampleRate * secondsPerBeat;
    needsUpdate = false;
    params.updateParameter(TRANSPORT::IDS::samples_per_beat, samplesPerBeat);
}

/*
* Synchronize the playhead state, ahead of calling getPosition()
*/
void CustomPlayHead::synchronizeState()
{
    if (*transportParams.reposition_flag)
    {
        changePosition();
    }

    hostInfo = *processor.getPlayHead()->getPosition();

    // assumption: all hosts (outside standalone) will provide at least the playing state
    if (isStandalone)
    {
        nextPlaying = *transportParams.playing;
        *transportParams.host_controls_playing = false;
    }
    else
    {
        nextPlaying = hostInfo.getIsPlaying();
        *transportParams.host_controls_playing = true;
    }

    // check if the playing state has changed
    if (isPlaying != nextPlaying)
    {
        isPlaying = !isPlaying;
        // set the playing state in the transport params
        //updateParameter(IDS::playing, isPlaying);
        *transportParams.playing = isPlaying;
        DBG("TParam: " << *transportParams.apvts.getRawParameterValue(TRANSPORT::IDS::playing));
    }

    // if the host provides a tempo, use it
    if (hostInfo.getBpm().hasValue())
    {
        nextTempo = *hostInfo.getBpm();
        *transportParams.host_controls_tempo = true;
    }
    else
    {
        nextTempo = transportParams.apvts.getRawParameterValue(TRANSPORT::IDS::tempo)->load();
        *transportParams.host_controls_tempo = false;
    }

    // check if the tempo has changed
    if (tempo != nextTempo)
    {
        tempo = nextTempo;
        needsUpdate = true;
        if (*transportParams.host_controls_tempo)
        {
            // set the tempo in the transport params
            transportParams.updateParameter(TRANSPORT::IDS::tempo, nextTempo);
        }
    };

    // check if the host has a time signature
    if (hostInfo.getTimeSignature().hasValue())
    {
        nextTimeSig = *hostInfo.getTimeSignature();
        *transportParams.host_controls_time_signature = true;
        //updateParameter(IDS::host_controls_time_sig, true);
        // TODO - need to know if all DAWs do this:
        //updateParameter(IDS::host_controls_tempo_speed, true);
        *transportParams.host_controls_tempo_speed = true;
        //updateParameter(IDS::time_sig_controls_tempo_speed, true);
        *transportParams.time_sig_controls_tempo_speed = true;
    }
    else
    {
        nextTimeSig.numerator = *transportParams.bar_length;
        nextTimeSig.denominator = *transportParams.beat_duration;
        //updateParameter(IDS::host_controls_time_sig, false);
        *transportParams.host_controls_time_signature = false;
        transportParams.updateParameter(TRANSPORT::IDS::host_controls_tempo_speed, false);
        *transportParams.host_controls_tempo_speed = false;
        transportParams.updateParameter(TRANSPORT::IDS::time_sig_controls_tempo_speed, false);
        *transportParams.time_sig_controls_tempo_speed = false;
    }

    // check if the time signature has changed
    if (timeSig.numerator != nextTimeSig.numerator || timeSig.denominator != nextTimeSig.denominator)
    {
        timeSig.numerator = nextTimeSig.numerator;
        timeSig.denominator = nextTimeSig.denominator;
        needsUpdate = true;

        // set the time signature in the transport params
        transportParams.updateParameter(TRANSPORT::IDS::bar_length, nextTimeSig.numerator);
        transportParams.updateParameter(TRANSPORT::IDS::beat_duration, nextTimeSig.denominator);
        DBG("Time Signature: " << timeSig.numerator << "/" << timeSig.denominator);
    }

    // check if tempo relative note duration has changed
    if (*transportParams.time_sig_controls_tempo_speed)
    {
        // set the tempo relative note duration based on the time signature
        nextTempoSpeed = 1.f / timeSig.denominator;
    }
    else
    {
        nextTempoSpeed = *transportParams.tempo_speed;
    }

    if (tempoSpeed != nextTempoSpeed)
    {
        tempoSpeed = nextTempoSpeed;
        needsUpdate = true;

        // set the tempo relative note duration in the transport params
        transportParams.updateParameter(TRANSPORT::IDS::tempo_speed, nextTempoSpeed);
    }

    // check if the host has position info (but only if the host has play control)
    if (*transportParams.host_controls_playing && (hostInfo.getPpqPosition().hasValue() || hostInfo.getTimeInSamples().hasValue() || hostInfo.getHostTimeNs().hasValue() || hostInfo.getTimeInSeconds().hasValue()))
    {
        *transportParams.host_controls_position = true;
    }
    else
    {
        *transportParams.host_controls_position = false;
    }

    if (needsUpdate)
    {
        recalculate(transportParams);
    }
}

/*
* Set position info
*/
void CustomPlayHead::updatePosition() const
{
    info.setHostTimeNs(timeNs);
    info.setBpm(tempo);
    info.setTimeInSamples(bufferPos);
    info.setTimeInSeconds(bufferPos / sampleRate);
    info.setPpqPosition(ppq);
    info.setIsPlaying(isPlaying);

    // report positions
    transportParams.setSamplePosition(bufferPos);
    transportParams.setPpq(ppq); // not using a cached value for ppq as it triggers a listener chain which can cause concurrent access errors on the listener list
    transportParams.setBarBeatDivPos(ppq);
}

/*
* Advance the playhead position based on the buffer size and host transport information
*/
void CustomPlayHead::advancePlayHead() const
{
    if (*transportParams.host_controls_playing && processor.getPlayHead()->getPosition().hasValue())
    {
        // Only advance if host is controlling position but time in samples is not available
        if (*transportParams.host_controls_position && !processor.getPlayHead()->getPosition()->getTimeInSamples().hasValue())
        {
            bufferPos += bufferSize;
            double beatsAdvanced = static_cast<double>(bufferSize) / samplesPerBeat;
            ppq += beatsAdvanced * beatsPerQuarterNote;
            timeNs = (bufferPos / sampleRate) * 1e9;
        }
    }
    else
    {
        // Standalone or internal transport
        bufferPos += bufferSize;
        double beatsAdvanced = static_cast<double>(bufferSize) / samplesPerBeat;
        ppq += beatsAdvanced * beatsPerQuarterNote;
        timeNs = (bufferPos / sampleRate) * 1e9;
    }
}