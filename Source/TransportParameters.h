/*
  ==============================================================================

    TransportParameters.h
    Created: 3 Oct 2024 11:37:13pm
    Author:  Nicholas

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "TransportConstants.h"
#include "ParameterGroup.h"

/// <summary>
/// Contains and initializes an AudioProcessorValueTreeState for the transport 
/// parameters of the plugin.
/// </summary>
struct TransportParameters :
    ParameterGroup
{

    TransportParameters(juce::UndoManager* undoManager, juce::AudioProcessorValueTreeState& apvts);
    ~TransportParameters();

    juce::CachedValue<bool> host_controls_playing {apvts.state, TRANSPORT::IDS::host_controls_play, undoManager, false};
    juce::CachedValue<bool> host_controls_position {apvts.state, TRANSPORT::IDS::host_controls_position, undoManager, false};
    juce::CachedValue<bool> host_controls_tempo {apvts.state, TRANSPORT::IDS::host_controls_tempo, undoManager, false};
    juce::CachedValue<bool> host_controls_tempo_speed {apvts.state, TRANSPORT::IDS::host_controls_tempo_speed, undoManager, false};
    juce::CachedValue<bool> host_controls_time_signature {apvts.state, TRANSPORT::IDS::host_controls_time_sig, undoManager, false};
    juce::CachedValue<bool> time_sig_controls_tempo_speed {apvts.state, TRANSPORT::IDS::time_sig_controls_tempo_speed, undoManager, false};
    
    juce::CachedValue<int> bar_length {apvts.state, TRANSPORT::IDS::bar_length, undoManager, 4};
    juce::CachedValue<int> beat_duration {apvts.state, TRANSPORT::IDS::beat_duration, undoManager, 4};
    juce::CachedValue<bool> playing {apvts.state, TRANSPORT::IDS::playing, undoManager, false};
    juce::CachedValue<bool> reposition_flag {apvts.state, TRANSPORT::IDS::reposition_flag, undoManager, false};

    // ppq is not a good candidate for a cached value, as it is frequently updated
    // juce::CachedValue<float> ppq{ apvts.state, TRANSPORT::IDS::ppq, undoManager, 0.f };

    // pos_* are not good candidates for cached values, as they are frequently updated
    juce::CachedValue<float> pos_bar{ apvts.state, TRANSPORT::IDS::pos_bar, undoManager, 0.f };
    juce::CachedValue<float> pos_beat{ apvts.state, TRANSPORT::IDS::pos_beat, undoManager, 0.f };
    juce::CachedValue<float> pos_div{ apvts.state, TRANSPORT::IDS::pos_div, undoManager, 0.f };

    juce::CachedValue<float> sample_rate {apvts.state, TRANSPORT::IDS::sample_rate, undoManager, 384000.f};
    juce::CachedValue<float> tempo {apvts.state, TRANSPORT::IDS::tempo, undoManager, 120.f};
    juce::CachedValue<float> tempo_speed {apvts.state, TRANSPORT::IDS::tempo_speed, undoManager, 0.25f};

    static std::unique_ptr<juce::AudioProcessorParameterGroup>  createParameters();

    float getPpq();
    void setPpq(float ppq);
    void setPos(float ppq, bool forceUpdate = false);

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportParameters)
};