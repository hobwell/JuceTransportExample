/*
  ==============================================================================

    GenericComponentAttachment.cpp
    Created: 3 May 2025 12:12:15pm
    Author:  Nicholas

  ==============================================================================
*/

#include "GenericComponentAttachment.h"

GenericComponentAttachment::GenericComponentAttachment(juce::AudioProcessorValueTreeState& state,
    const juce::String& paramID,
    juce::Component& componentToBind,
    ParameterToUI onParameterChanged,
    UIToParameter onUserChanged
) : 
    component(componentToBind),
    setUI(std::move(onParameterChanged))
{
    parameter = dynamic_cast<juce::RangedAudioParameter*>(state.getParameter(paramID));
    jassert(parameter != nullptr);

    parameter->addListener(this);

    // Hook the UI component to call into the parameter update
    onUserChanged([this] (float newValue)
        {
            if (parameter != nullptr)
                parameter->setValueNotifyingHost(parameter->convertTo0to1(newValue));
        });

    // Initial sync
    if (parameter != nullptr)
    {
        float value = parameter->convertFrom0to1(parameter->getValue());
        juce::MessageManager::callAsync([this, value] { setUI(value); });
    }
}

GenericComponentAttachment::~GenericComponentAttachment()
{
    if (parameter != nullptr)
    {
        parameter->removeListener(this);
    }
}

void GenericComponentAttachment::parameterValueChanged(int /*parameterIndex*/, float newValue)
{
    if (parameter == nullptr)
        return;

    // Convert normalized [0, 1] back to real value and update UI
    float unnormalized = parameter->convertFrom0to1(newValue);

    juce::MessageManager::callAsync([this, unnormalized]
        {
            setUI(unnormalized);
        });
}
