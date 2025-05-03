/*
  ==============================================================================

    Fonts.h
    Created: 26 Jun 2024 9:25:47pm
    Author:  Nicholas

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Fonts
{
public:
    juce::Font& getFontAwesome();

    Fonts();
    ~Fonts();

private:
    juce::Font fontAwesome;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Fonts)
};