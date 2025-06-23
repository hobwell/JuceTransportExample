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
/// parameters of the plugin, with flexible state rebinding.
/// </summary>
struct TransportParameters :
    ParameterGroup
{

    TransportParameters(juce::UndoManager* undoManager, juce::AudioProcessorValueTreeState& apvts);
    ~TransportParameters();

    void attachToState(juce::ValueTree& state, juce::UndoManager* undoManager);

    std::unique_ptr<juce::CachedValue<bool>> host_controls_playing;
    std::unique_ptr<juce::CachedValue<bool>> host_controls_position;
    std::unique_ptr<juce::CachedValue<bool>> host_controls_tempo;
    std::unique_ptr<juce::CachedValue<bool>> host_controls_tempo_speed;
    std::unique_ptr<juce::CachedValue<bool>> host_controls_time_signature;
    std::unique_ptr<juce::CachedValue<bool>> time_sig_controls_tempo_speed;
    
    std::unique_ptr<juce::CachedValue<int>> bar_length;
    std::unique_ptr<juce::CachedValue<int>> beat_duration;
    std::unique_ptr<juce::CachedValue<bool>> playing;
    std::unique_ptr<juce::CachedValue<bool>> reposition_flag;

    // ppq is not a good candidate for a cached value, as it is frequently updated
    // juce::CachedValue<float> ppq{ apvts.state, TRANSPORT::IDS::ppq, undoManager, 0.f };

    // pos_* are not good candidates for cached values, as they are frequently updated
    std::unique_ptr<juce::CachedValue<float>> pos_bar;
    std::unique_ptr<juce::CachedValue<float>> pos_beat;
    std::unique_ptr<juce::CachedValue<float>> pos_div;

    std::unique_ptr<juce::CachedValue<float>> sample_rate;
    std::unique_ptr<juce::CachedValue<float>> tempo;
    std::unique_ptr<juce::CachedValue<float>> tempo_speed;  // what the tempo is relative to (e.g. 1/4 is a quarter note, 1/8 is an eighth note, etc.)

    static std::unique_ptr<juce::AudioProcessorParameterGroup>  createParameters();

    float getPpq();
    void setPpq(float ppq);
    void setPos(float ppq, bool forceUpdate = false);

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportParameters)
};