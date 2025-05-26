/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "TransportConstants.h"

//==============================================================================
TransportExampleAudioProcessorEditor::TransportExampleAudioProcessorEditor(TransportExampleAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	// create the transport interface
	transportInterface = std::make_unique<UI_Transport>(*p.transportParams);
	setResizable(true, true);

	setSize(TRANSPORT::LAYOUT::WINDOW::WIDTH, TRANSPORT::LAYOUT::WINDOW::HEIGHT);

	addAndMakeVisible(transportInterface.get());
}

TransportExampleAudioProcessorEditor::~TransportExampleAudioProcessorEditor() {}

//==============================================================================
void TransportExampleAudioProcessorEditor::paint(juce::Graphics& g) {}

void TransportExampleAudioProcessorEditor::resized()
{
	// update subcomponents
	transportInterface->setBounds(getLocalBounds());
}
