/*
  ==============================================================================

    GenericComponentAttachment.h
    Created: 3 May 2025 12:12:15pm
    Author:  Nicholas

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>

class GenericComponentAttachment : private juce::AudioProcessorParameter::Listener
{
public:
    using ParameterToUI = std::function<void(float)>;
    using UIToParameter = std::function<void(std::function<void(float)>)>;

    GenericComponentAttachment(juce::AudioProcessorValueTreeState& state,
        const juce::String& paramID,
        juce::Component& componentToBind,
        ParameterToUI onParameterChanged,
        UIToParameter onUserChanged);

    ~GenericComponentAttachment() override;

private:
    void parameterValueChanged(int, float newValue) override;
    void parameterGestureChanged(int, bool) override {}

    juce::Component& component;
    juce::RangedAudioParameter* parameter = nullptr;
    ParameterToUI setUI;
};
