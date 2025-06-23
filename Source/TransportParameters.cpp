/*
  ==============================================================================

    TransportParameters.cpp
    Created: 3 Oct 2024 11:37:13pm
    Author:  Nicholas

  ==============================================================================
*/

#include "TransportParameters.h"

// struct TransportParameters

TransportParameters::TransportParameters(juce::UndoManager* undoManager, juce::AudioProcessorValueTreeState& apvts) :
    ParameterGroup(undoManager, apvts) {
    attachToState(apvts.state, undoManager);
}

TransportParameters::~TransportParameters() {}

void TransportParameters::attachToState(juce::ValueTree& state, juce::UndoManager* undoManager)
{
    beat_duration = std::make_unique<juce::CachedValue<int>>  (state, TRANSPORT::IDS::beat_duration, undoManager, 4);
    bar_length = std::make_unique<juce::CachedValue<int>>     (state, TRANSPORT::IDS::bar_length, undoManager, 4);
    host_controls_playing = std::make_unique<juce::CachedValue<bool>>(state, TRANSPORT::IDS::host_controls_play, undoManager, false);
    host_controls_position = std::make_unique<juce::CachedValue<bool>>(state, TRANSPORT::IDS::host_controls_position, undoManager, false);
    host_controls_tempo = std::make_unique<juce::CachedValue<bool>>(state, TRANSPORT::IDS::host_controls_tempo, undoManager, false);
    host_controls_tempo_speed = std::make_unique<juce::CachedValue<bool>>(state, TRANSPORT::IDS::host_controls_tempo_speed, undoManager, false);
    host_controls_time_signature = std::make_unique<juce::CachedValue<bool>>(state, TRANSPORT::IDS::host_controls_time_sig, undoManager, false);
    reposition_flag = std::make_unique<juce::CachedValue<bool>>(state, TRANSPORT::IDS::reposition_flag, undoManager, false);
    playing = std::make_unique<juce::CachedValue<bool>>(state, TRANSPORT::IDS::playing, undoManager, false);
    time_sig_controls_tempo_speed = std::make_unique<juce::CachedValue<bool>>(state, TRANSPORT::IDS::time_sig_controls_tempo_speed, undoManager, false);
    pos_bar = std::make_unique<juce::CachedValue<float>>(state, TRANSPORT::IDS::pos_bar, undoManager, 1.f);
    pos_beat = std::make_unique<juce::CachedValue<float>>(state, TRANSPORT::IDS::pos_beat, undoManager, 1.f);
    pos_div = std::make_unique<juce::CachedValue<float>>(state, TRANSPORT::IDS::pos_div, undoManager, 1.f);
    sample_rate = std::make_unique<juce::CachedValue<float>>(state, TRANSPORT::IDS::sample_rate, undoManager, 48000.f);
    tempo = std::make_unique<juce::CachedValue<float>>(state, TRANSPORT::IDS::tempo, undoManager, 120.f);
    tempo_speed = std::make_unique<juce::CachedValue<float>>(state, TRANSPORT::IDS::tempo_speed, undoManager, 0.25f);
}

std::unique_ptr<juce::AudioProcessorParameterGroup>  TransportParameters::createParameters()
{
    // TODO - make groups instead
    auto group = std::make_unique<juce::AudioProcessorParameterGroup> (
        TRANSPORT::IDS::transport_group,        // group ID (must be unique)
        TRANSPORT::LABELS::transport_group,     // group name (for display)
        "."                                     // group seperator
    );

    group->addChild (std::make_unique<juce::AudioParameterInt> (juce::ParameterID {TRANSPORT::IDS::beat_duration, 1}, TRANSPORT::LABELS::beat_duration, 1, 16, 4));
    group->addChild (std::make_unique<juce::AudioParameterInt> (juce::ParameterID {TRANSPORT::IDS::bar_length, 1}, TRANSPORT::LABELS::bar_length, 1, 99, 4));
    group->addChild (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::host_controls_play, 1}, TRANSPORT::LABELS::host_controls_play_state, false));
    group->addChild (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::host_controls_position, 1}, TRANSPORT::LABELS::host_controls_position_state, false));
    group->addChild (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::host_controls_tempo, 1}, TRANSPORT::LABELS::host_controls_tempo_state, false));
    group->addChild (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::host_controls_tempo_speed, 1}, TRANSPORT::LABELS::host_controls_tempo_speed, false));
    group->addChild (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::host_controls_time_sig, 1}, TRANSPORT::LABELS::host_controls_time_signature_state, false));
    group->addChild (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::reposition_flag, 1}, TRANSPORT::LABELS::reposition_flag, false));
    group->addChild (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::playing, 1}, TRANSPORT::LABELS::playing, false));
    group->addChild (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::time_sig_controls_tempo_speed, 1}, TRANSPORT::LABELS::time_sig_controls_tempo_speed, false));
    group->addChild (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {TRANSPORT::IDS::pos_bar, 1}, TRANSPORT::LABELS::pos_bar, 1.f, 99999.f, 1.f));
    group->addChild (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {TRANSPORT::IDS::pos_beat, 1}, TRANSPORT::LABELS::pos_beat, 1.f, 99.f, 1.f));
    group->addChild (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {TRANSPORT::IDS::pos_div, 1}, TRANSPORT::LABELS::pos_div, 1.f, 256.f, 1.f));
    group->addChild (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {TRANSPORT::IDS::ppq, 1}, TRANSPORT::LABELS::ppq, -99999.f, 99999.f, 0)); // 27+hrs at 60bpm, 13+hrs @ 120bpm etc.
    group->addChild (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {TRANSPORT::IDS::sample_rate, 1}, TRANSPORT::LABELS::sample_rate, 0.f, 384000.f, 48000.f));

    // need to specify the step size for the tempo so that it can be exact
    juce::NormalisableRange<float> tempoSteppedRange(1.00, 999.0f, 0.01f);
    group->addChild (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID {TRANSPORT::IDS::tempo, 1},
        TRANSPORT::LABELS::tempo,
        tempoSteppedRange,
        120.f
    ));

    // need to specify the step size for the tempo duration so that it can be exact
    juce::NormalisableRange<float> tempoDurationSteppedRange(0.0078125f, 1.0f, 0.00390625f); // step size is 1/256
    group->addChild (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID {TRANSPORT::IDS::tempo_speed, 1},
        TRANSPORT::LABELS::tempo_speed,
        tempoDurationSteppedRange,
        0.25f
    ));

    return group;
}

// given a duration (relative to a whole note) calculate the number of samples, given the current tempo, tempo speed and sample rate
double TransportParameters::calculateTempoRelativeSamples(float duration) const
{
    // Get the current tempo and tempo speed
    float currentTempo = *tempo;
    float currentTempoSpeed = *tempo_speed;
    // Calculate the duration in seconds for the given duration relative to a whole note
    double durationInSeconds = (60.0 / currentTempo) * duration * currentTempoSpeed;
    // Calculate the number of samples based on the sample rate
    double sampleRate = *sample_rate;
    return durationInSeconds * sampleRate;
}

float TransportParameters::getPpq()
{
    return *apvts.getRawParameterValue(TRANSPORT::IDS::ppq);
}

void TransportParameters::setPpq(float ppq)
{
    apvts.getRawParameterValue(TRANSPORT::IDS::ppq)->store(ppq);
}

void TransportParameters::setPos(float ppq, bool forceUpdate)
{
    // Calculate the number of subdivisions per beat based on beat_duration
    int subDivisionsPerBeat = 16 / *beat_duration;  // This adjusts based on beat duration

    // Calculate the scaler for converting PPQ to beats based on beat duration
    float quarterNotesPerBeat = 4.0f / *beat_duration; // Adjust beat rate based on beat duration

    // convert ppq to number of total beats, based on the beat duration
    float beatPosition = ppq / quarterNotesPerBeat;

    updateParameter(TRANSPORT::IDS::ppq, ppq);
    updateParameter(TRANSPORT::IDS::pos_bar, 1 + ((int) beatPosition / *bar_length), forceUpdate);
    updateParameter(TRANSPORT::IDS::pos_beat, 1 + ((int) beatPosition % *bar_length), forceUpdate);
    updateParameter(TRANSPORT::IDS::pos_div, 1 + ((int) (beatPosition * subDivisionsPerBeat) % subDivisionsPerBeat), forceUpdate);
}
