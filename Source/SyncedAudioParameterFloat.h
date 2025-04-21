/*
  ==============================================================================

    SyncedAudioParameterFloat.h
    Created: 20 Apr 2025 9:32:00pm
    Author:  Nicholas

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>

class SyncedAudioParameterFloat : public juce::AudioParameterFloat
{
public:
    SyncedAudioParameterFloat(const juce::String& parameterID,
        const juce::String& name,
        juce::NormalisableRange<float> range,
        float defaultValue,
        juce::ValueTree& state,
        juce::UndoManager* undo = nullptr);

    void setValueNotifyingHost(float newValueNormalized);

private:
    juce::ValueTree& tree;
    juce::Identifier id;
    juce::UndoManager* undoManager;
};
