/*
  ==============================================================================

    KeyValueParameter.h
    Created: 18 Apr 2025 2:34:25pm
    Author:  Nicholas

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "KeyValuePair.h"

class KeyValueParameter : public juce::AudioParameterChoice
{
public:
    KeyValueParameter(const juce::String& id,
        const juce::String& name,
        const std::vector<KeyValuePair>& options)
        : juce::AudioParameterChoice(id, name, createChoices(options), 0),
        options(options)
    {
    }

private:
    const std::vector<KeyValuePair>& options;

    // Helper function to create the choices string array
    static juce::StringArray createChoices(const std::vector<KeyValuePair>& options)
    {
        juce::StringArray choices;
        for (const auto& option : options)
            choices.add(option.key);
        return choices;
    }
};