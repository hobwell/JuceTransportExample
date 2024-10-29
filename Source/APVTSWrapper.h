/*
  ==============================================================================

    AVPTSWrapper.h
    Created: 14 Oct 2024 2:52:50pm
    Author:  Nicholas

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "TransportTree.h"

/// <summary>
/// A wapper class for the AudioProcessorValueTreeState that provides a more
/// convenient way to access the parameters in the tree and a mechanism to 
/// raise events in response to changes to those parameters.
/// </summary>
/// <see href="https://www.classcentral.com/classroom/youtube-vars-values-and-valuetrees-state-management-in-juce-jelle-bakker-adc23-320032">ADC23 - Jelle Bakker: Vars, Values and ValueTrees: Managing State with JUCE</see>
struct APVTSWrapper : juce::ValueTree::Listener
{
    juce::AudioProcessorValueTreeState& apvts;
    TransportTree& tree;
    juce::UndoManager* undoManager;

    juce::CachedValue<int> bar_length{ apvts.state, IDS::bar_length, undoManager, 4 };
    juce::CachedValue<int> beat_duration{ apvts.state, IDS::beat_duration, undoManager, 4 };
    juce::CachedValue<bool> host_controls_playing{ apvts.state, IDS::host_controls_play, undoManager, false };
    juce::CachedValue<bool> host_controls_position{ apvts.state, IDS::host_controls_position, undoManager, false };
    juce::CachedValue<bool> host_controls_tempo{ apvts.state, IDS::host_controls_tempo, undoManager, false };
    juce::CachedValue<bool> host_controls_time_signature{ apvts.state, IDS::host_controls_time_sig, undoManager, false };
    juce::CachedValue<bool> playing{ apvts.state, IDS::playing, undoManager, false };
    juce::CachedValue<bool> rewind_flag{ apvts.state, IDS::rewind_flag, undoManager, false };
    juce::CachedValue<float> ppq{ apvts.state, IDS::ppq, undoManager, 0.f };
    juce::CachedValue<float> sample_rate{ apvts.state, IDS::sample_rate, undoManager, 384000 };
    juce::CachedValue<float> tempo{ apvts.state, IDS::tempo, undoManager, 120 };

    std::function<void(bool)> onHostControlsPlayingChanged;
    std::function<void(bool)> onHostControlsPositionChanged;
    std::function<void(bool)> onHostControlsTempoChanged;
    std::function<void(bool)> onHostControlsTimeSignatureChanged;
    std::function<void(float)> onTempoChanged;

    APVTSWrapper(TransportTree* transportTree, juce::UndoManager* undoManager);
    ~APVTSWrapper();

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(APVTSWrapper)
};