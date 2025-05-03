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

	params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID {IDS::beat_duration, 1}, LABELS::beat_duration, 1, 16, 4));
	params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID {IDS::bar_length, 1}, LABELS::bar_length, 1, 99, 4));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {IDS::host_controls_play, 1}, LABELS::host_controls_play_state, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {IDS::host_controls_position, 1}, LABELS::host_controls_position_state, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {IDS::host_controls_tempo, 1}, LABELS::host_controls_tempo_state, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {IDS::host_controls_tempo_speed, 1}, LABELS::host_controls_tempo_speed, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {IDS::host_controls_time_sig, 1}, LABELS::host_controls_time_signature_state, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {IDS::rewind_flag, 1}, LABELS::rewind_flag, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {IDS::playing, 1}, LABELS::playing, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {IDS::time_sig_controls_tempo_speed, 1}, LABELS::time_sig_controls_tempo_speed, false));
	params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {IDS::pos_bar, 1}, LABELS::pos_bar, 1.f, 99999.f, 1.f));
	params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {IDS::pos_beat, 1}, LABELS::pos_beat, 1.f, 99.f, 1.f));
	params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {IDS::pos_div, 1}, LABELS::pos_div, 1.f, 256.f, 1.f));
	params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {IDS::ppq, 1}, LABELS::ppq, -99999.f, 99999.f, 0)); // 27+hrs at 60bpm, 13+hrs @ 120bpm etc.
	params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {IDS::sample_rate, 1}, LABELS::sample_rate, 0.f, 384000.f, 48000.f));
	
	// need to specify the step size for the tempo so that it can be exact
	juce::NormalisableRange<float> tempoSteppedRange(1.00, 999.0f, 0.01f);
	params.push_back (std::make_unique<juce::AudioParameterFloat> (
		juce::ParameterID {IDS::tempo, 1}, 
		LABELS::tempo, 
		tempoSteppedRange, 
		120.f
	));

    // need to specify the step size for the tempo duration so that it can be exact
    juce::NormalisableRange<float> tempoDurationSteppedRange(0.0078125f, 1.0f, 0.00390625f); // step size is 1/256
	params.push_back(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID {IDS::tempo_speed, 1}, 
		LABELS::tempo_speed, 
		tempoDurationSteppedRange, 
		0.25f
	));
	//params.push_back(std::make_unique<SyncedAudioParameterFloat>(IDS::tempo_speed, LABELS::tempo_speed, tempoSteppedRange, 0.25f, apvts.state));

	return { params.begin(), params.end() };
}

std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> TransportTree::createButtonAttachment(const juce::String& parameterID, juce::Button& button)
{
	return std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, parameterID, button);
}

std::unique_ptr<GenericComponentAttachment> TransportTree::createGenericAttachment(const juce::String& parameterID, juce::Component& component, std::function<void(float)> paramToUi, std::function<void(std::function<void(float)>)> uiToParam)
{
	return std::make_unique<GenericComponentAttachment>(apvts, parameterID, component, paramToUi, uiToParam);
}

std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> TransportTree::createSliderAttachment(const juce::String& parameterID, juce::Slider& slider)
{
	return std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, parameterID, slider);
}