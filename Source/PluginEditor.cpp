/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "TransportLayoutConstants.h"

//==============================================================================
TransportExampleAudioProcessorEditor::TransportExampleAudioProcessorEditor(TransportExampleAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	// create the transport interface
	transportInterface = std::make_unique<UI_Transport>(*p.transportParams);
	setResizable(true, true);

	setSize(LAYOUT::TRANSPORT::WINDOW::WIDTH, LAYOUT::TRANSPORT::WINDOW::HEIGHT);

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
