/*
  ==============================================================================

    KeyValuePair.h
    Created: 18 Apr 2025 1:10:10pm
    Author:  Nicholas

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct KeyValuePair
{
    KeyValuePair() = default;
    KeyValuePair(const std::string& key, const juce::var& value) : key(key), value(value) {}
    std::string key;
    juce::var value;
    bool operator==(const KeyValuePair& other) const
    {
        return key == other.key && value == other.value;
    }
};