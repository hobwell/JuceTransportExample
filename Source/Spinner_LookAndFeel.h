/*
  ==============================================================================

    Spinner_LookAndFeel.h
    Created: 14 Jul 2024 6:20:45pm
    Author:  Nicholas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Fonts.h"

class Spinner_LookAndFeel {
public:
    Spinner_LookAndFeel();
    ~Spinner_LookAndFeel();

    juce::LookAndFeel_V4 active;
    juce::LookAndFeel_V4 inactive;
private:
    int horizontalPadding = 5;
    int verticalPadding = 4;
	int fontSize = 16;
};