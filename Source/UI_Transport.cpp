/*
  ==============================================================================

	UI_Transport.cpp
	Created: 14 Sep 2024 1:08:24pm
	Author:  Nicholas

  ==============================================================================
*/

#include <JuceHeader.h>
#include "UI_Transport.h"



//==============================================================================
UI_Transport::UI_Transport(APVTSWrapper& transportWrapper) : transportWrapper(transportWrapper)
{
	// when the spinner value changes, update the tree
	tempoSpinner.setValue(transportWrapper.tempo);
	addAndMakeVisible(tempoSpinner);
	// connect the tempo slider to the "tempo" audio parameter
	// this will adjust the slider's range to match the parameter's range - it's also supposed to bind the UI value to the parameter value, but it doesn't seem to do that for some reason
	tempoAttachment = transportWrapper.tree.createSliderAttachment(IDS::tempo, tempoSpinner);
	
	// if the arbiter of the tempo changes, re-initialize the tempo setup
	tempoSetup(transportWrapper.host_controls_tempo);

	btnRewind.setButtonText(UNICON::rewind);
	btnRewind.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
	btnRewind.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
	btnRewind.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::white);
	btnRewind.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::orange);
	addAndMakeVisible(btnRewind);
	btnRewind.onClick = [this]
		{
			this->transportWrapper.rewind_flag = true;
		};

	btnPlay.setButtonText(UNICON::play);
	btnPlay.setClickingTogglesState(true);
	btnPlay.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
	btnPlay.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
	btnPlay.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::white);
	btnPlay.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::green);
	addAndMakeVisible(btnPlay);

	// connect the play button to the "playing" audio parameter
	buttonAttachment = transportWrapper.tree.createButtonAttachment(IDS::playing, btnPlay);

	// initialize the playing setup
	playControlSetup(transportWrapper.host_controls_playing);

	// todo: implement transport position
	transportPositionLabel.setText("1.1.1", juce::dontSendNotification);
	addAndMakeVisible(transportPositionLabel);

}

UI_Transport::~UI_Transport() {}

juce::String UI_Transport::getPosition()
{
	ppq = transportWrapper.ppq;
	int bars = 1 + ((int)ppq / transportWrapper.beat_duration);
	int beats = 1 + ((int)ppq % transportWrapper.bar_length);
	int divisions = 1 + ((int)(ppq * 4) % 4);
	return juce::String(bars) + "." + juce::String(beats) + "." + juce::String(divisions);
}

void UI_Transport::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

	g.setColour(juce::Colours::grey);

	g.setColour(juce::Colours::white);

	g.setFont(14.0f);
}

void UI_Transport::playControlSetup(bool hostControls)
{
	btnPlay.setEnabled(!hostControls);
	btnRewind.setEnabled(!hostControls);

	btnPlay.onClick = [this]
	{
		// Interestingly, this gets called when the attached parameter changes even if the button is not clicked and even if the button is disabled
		if (btnPlay.getToggleState()) {
			btnPlay.setButtonText(UNICON::stop);
		}
		else {
			btnPlay.setButtonText(UNICON::play);
		}
		if (!this->transportWrapper.host_controls_playing) {
			// also set the transport playing state
			this->transportWrapper.playing = btnPlay.getToggleState();
		}
	};
}

void UI_Transport::resized()
{
	auto body = getLocalBounds();
	int p = 5; // padding
	int pp = p * 2; // double padding - added to width and height to account for padding on all sides
	int w = getWidth() - pp;
	int h = getHeight() - pp;

	// define the inner area of the component
	auto area = body.removeFromLeft(w + pp).removeFromTop(h + pp).reduced(p);

	btnRewind.setBounds(area.removeFromLeft(w / 14));
	btnPlay.setBounds(area.removeFromLeft(w / 14));
	tempoSpinner.setBounds(area.removeFromLeft(w / 7));
	transportPositionLabel.setBounds(area.removeFromLeft(w * 2. / 7.));
}

/// <summary>
/// Attaches handlers to the transport wrapper or the spinner, depending on the arbiter of the tempo
/// </summary>
/// <param name="hostControls"></param>
void UI_Transport::tempoSetup(bool hostControls)
{
	if (hostControls) {
		tempoSpinner.setEnabled(false);
		// we want to be able to match the host, so we'll allow for 2 decimals of precision
		tempoSpinner.setNumDecimalPlacesToDisplay(2);
		tempoSpinner.onValueChange= nullptr;
	}
	else {
		// when there is no host, we can set the tempo to an integer value
		tempoSpinner.setNumDecimalPlacesToDisplay(0);
		tempoSpinner.setEnabled(true);
		tempoSpinner.onValueChange = [&]
		{
			// when the spinner value changes, update the tree
			// this is causing access violation errors
			transportWrapper.tempo = (int)round(tempoSpinner.getValue()); // BUG: this assignment is causing the listener to trigger, which is sometimes causing an error
		};
	}
}

void UI_Transport::timerCallback()
{
	tempoSpinner.timerCallback();
	transportPositionLabel.setText(getPosition(), juce::dontSendNotification);
	if (transportWrapper.host_controls_tempo) {
		tempoSpinner.setValue(transportWrapper.tempo, juce::NotificationType::dontSendNotification); // don't send notification, we're updating in response to a value change - the slider attachment would trigger a second update
	}
}