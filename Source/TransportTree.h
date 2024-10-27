/*
  ==============================================================================

	TransportTree.h
	Created: 3 Oct 2024 11:37:13pm
	Author:  Nicholas

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "StringConstants.h"

/// <summary>
/// Contains and initializes an AudioProcessorValueTreeState for the transport 
/// parameters of the plugin.
/// </summary>
struct TransportTree
{
	juce::AudioProcessorValueTreeState apvts;
	juce::AudioProcessorValueTreeState::ParameterLayout parameters;
	
	TransportTree(juce::AudioProcessor& processor, juce::UndoManager* undoManager, const juce::Identifier& valueTreeType);
	~TransportTree();

	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> createButtonAttachment(const juce::String& parameterID, juce::Button& button);
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> createSliderAttachment(const juce::String& parameterID, juce::Slider& slider);
private:
	juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportTree)
};