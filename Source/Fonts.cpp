/*
  ==============================================================================

	Fonts.cpp
	Created: 26 Jun 2024 9:25:47pm
	Author:  Nicholas

  ==============================================================================
*/

#include "Fonts.h"

Fonts::Fonts() {
	juce::Typeface::Ptr test = juce::Typeface::createSystemTypefaceFor(BinaryData::FA6FreeSolid_otf, BinaryData::FA6FreeSolid_otfSize);
	juce::String teststr = test->getName();
	fontAwesome = juce::Font(test);
}

Fonts::~Fonts() {}

juce::Font& Fonts::getFontAwesome() {
	return fontAwesome;
}