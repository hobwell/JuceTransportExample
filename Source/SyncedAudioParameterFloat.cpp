/*
  ==============================================================================

    SyncedAudioParameterFloat.cpp
    Created: 20 Apr 2025 9:32:00pm
    Author:  Nicholas

  ==============================================================================
*/

#include "SyncedAudioParameterFloat.h"

SyncedAudioParameterFloat::SyncedAudioParameterFloat(const juce::String& parameterID,
    const juce::String& name,
    juce::NormalisableRange<float> range,
    float defaultValue,
    juce::ValueTree& state,
    juce::UndoManager* undo)
    : juce::AudioParameterFloat(parameterID, name, range, defaultValue),
    tree(state), id(parameterID), undoManager(undo)
{
}

void SyncedAudioParameterFloat::setValueNotifyingHost(float newValueNormalized)
{
    AudioParameterFloat::setValueNotifyingHost(newValueNormalized);
    tree.setProperty(id, get(), undoManager);
}
