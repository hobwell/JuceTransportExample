/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TransportExampleAudioProcessorEditor::TransportExampleAudioProcessorEditor(TransportExampleAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	// create the transport interface
	transportInterface = std::make_unique<UI_Transport>(*p.transportWrapper);
	setResizable(true, true);

	setSize(width, height);

	addAndMakeVisible(transportInterface.get());

	startTimer(33); // 33ms ~= 30fps
}

TransportExampleAudioProcessorEditor::~TransportExampleAudioProcessorEditor() {}

//==============================================================================
void TransportExampleAudioProcessorEditor::paint(juce::Graphics& g) {}

void TransportExampleAudioProcessorEditor::resized()
{
	// update subcomponents
	transportInterface->setBounds(getLocalBounds());
}

void TransportExampleAudioProcessorEditor::timerCallback()
{
	// update subcomponents
	transportInterface->timerCallback();
}