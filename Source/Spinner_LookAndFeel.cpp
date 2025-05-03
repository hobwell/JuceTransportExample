/*
  ==============================================================================

	Spinner_LookAndFeel.cpp
	Created: 14 Jul 2024 6:20:45pm
	Author:  Nicholas

  ==============================================================================
*/

#include "Spinner_LookAndFeel.h"

Spinner_LookAndFeel::Spinner_LookAndFeel() {
	inactive.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
	inactive.setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
	inactive.setColour(juce::Label::textColourId, juce::Colours::white);

	active.setColour(juce::Label::backgroundColourId, juce::Colours::black);
	active.setColour(juce::Label::outlineColourId, juce::Colours::darkslategrey);
	active.setColour(juce::Label::textColourId, juce::Colours::orange);

	inactive.setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentBlack);
	inactive.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
	inactive.setColour(juce::TextEditor::textColourId, juce::Colours::white);

	active.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
	active.setColour(juce::TextEditor::outlineColourId, juce::Colours::darkslategrey);
	active.setColour(juce::TextEditor::textColourId, juce::Colours::orange);
	active.setColour(juce::TextEditor::highlightedTextColourId, juce::Colours::white);
	active.setColour(juce::TextEditor::highlightColourId, juce::Colours::darkslategrey);
	active.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::orange);
}

Spinner_LookAndFeel::~Spinner_LookAndFeel() {}