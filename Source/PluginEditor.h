/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI_Transport.h"

//==============================================================================
/**
*/
class TransportExampleAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
	TransportExampleAudioProcessorEditor(TransportExampleAudioProcessor&);
	~TransportExampleAudioProcessorEditor() override;

	//==============================================================================
	int height = 50;
	int width = 660;

	void paint(juce::Graphics&) override;
	void resized() override;
	void timerCallback() override;
private:
	/// <summary>
	/// Component containing the user interface
	/// </summary>
	std::unique_ptr<UI_Transport> transportInterface; // Q: Why did I make this a unique pointer?  A: The unique pointer will automatically delete the UI object when the plugin is deleted.

	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	TransportExampleAudioProcessor& audioProcessor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportExampleAudioProcessorEditor)
};
