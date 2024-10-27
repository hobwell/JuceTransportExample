/*
  ==============================================================================

	TransportTree.cpp
	Created: 3 Oct 2024 11:37:13pm
	Author:  Nicholas

  ==============================================================================
*/

#include "TransportTree.h"

// struct TransportTree

TransportTree::TransportTree(juce::AudioProcessor& processor, juce::UndoManager* undoManager, const juce::Identifier& valueTreeType) :
	parameters(createParameters()),
	apvts(processor, undoManager, valueTreeType, createParameters())
{}

TransportTree::~TransportTree() {}

juce::AudioProcessorValueTreeState::ParameterLayout TransportTree::createParameters()
{
	std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

	params.push_back(std::make_unique<juce::AudioParameterInt>(IDS::beat_duration, LABELS::beat_duration, 1, 16, 4));
	params.push_back(std::make_unique<juce::AudioParameterInt>(IDS::bar_length, LABELS::bar_length, 1, 99, 4));
	params.push_back(std::make_unique<juce::AudioParameterBool>(IDS::host_controls_play, LABELS::host_controls_play_state, false));
	params.push_back(std::make_unique<juce::AudioParameterBool>(IDS::host_controls_position, LABELS::host_controls_position_state, false));
	params.push_back(std::make_unique<juce::AudioParameterBool>(IDS::host_controls_tempo, LABELS::host_controls_tempo_state, false));
	params.push_back(std::make_unique<juce::AudioParameterBool>(IDS::host_controls_time_sig, LABELS::host_controls_time_signature_state, false));
	params.push_back(std::make_unique<juce::AudioParameterBool>(IDS::rewind_flag, LABELS::rewind_flag, false));
	params.push_back(std::make_unique<juce::AudioParameterBool>(IDS::playing, LABELS::playing, false));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(IDS::ppq, LABELS::ppq, -99999.f, 99999.f, 0)); // 27+hrs at 60bpm, 13+hrs @ 120bpm etc.
	params.push_back(std::make_unique<juce::AudioParameterFloat>(IDS::sample_rate, LABELS::sample_rate, 0.f, 384000.f, 48000.f));
	params.push_back(std::make_unique<juce::AudioParameterFloat>(IDS::tempo, LABELS::tempo, 20.f, 999.f, 120.f));

	return { params.begin(), params.end() };
}


std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> TransportTree::createButtonAttachment(const juce::String& parameterID, juce::Button& button)
{
	return std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, parameterID, button);
}

std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> TransportTree::createSliderAttachment(const juce::String& parameterID, juce::Slider& slider)
{
	return std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, parameterID, slider);
}