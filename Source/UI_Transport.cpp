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
UI_Transport::UI_Transport(ApvtsWrapper& transportWrapper) : transportWrapper(transportWrapper)
{
	// when the spinner value changes, update the tree
	spinTempo.setValue(transportWrapper.tempo);
	addAndMakeVisible(spinTempo);

	// connect the tempo slider to the "tempo" audio parameter
	// this will adjust the slider's range to match the parameter's range - it's also supposed to bind the UI value to the parameter value, but it doesn't seem to do that for some reason
	attach_Tempo = transportWrapper.tree.createSliderAttachment(IDS::tempo, spinTempo);

	spinTempoDuration.setValueMap(&TEMPO::duration_options);
	attach_TempoOptions = transportWrapper.tree.createSliderAttachment(IDS::tempo_duration, spinTempoDuration);
    spinTempoDuration.setValueFromKey("x");
	addAndMakeVisible(spinTempoDuration);

	// if the arbiter of the tempo changes, re-initialize the tempo setup
	setupTempo(transportWrapper.host_controls_tempo);

	spinBarLength.setValue(transportWrapper.bar_length);
	addAndMakeVisible(spinBarLength);
	attach_BarLength = transportWrapper.tree.createSliderAttachment(IDS::bar_length, spinBarLength);

	lblTimeSigSep.setText("/", juce::dontSendNotification);
	addAndMakeVisible(lblTimeSigSep);

	spinBeatLength.setValue(transportWrapper.beat_duration);
	addAndMakeVisible(spinBeatLength);
	attach_BeatLength = transportWrapper.tree.createSliderAttachment(IDS::beat_duration, spinBeatLength);

	setupTimeSignature(transportWrapper.host_controls_time_signature);
		
	spinBars.safeSetRange(1, 9999, 1);
	spinBars.setValue(1);
	addAndMakeVisible(spinBars);

	spinBeats.safeSetRange(1, spinBarLength.getValue(), 1);
	spinBeats.setValue(1);
	addAndMakeVisible(spinBeats);

	spinSubdiv.safeSetRange(1, (int)(16.f / spinBeatLength.getValue()),1);
	spinSubdiv.setValue(1);
	addAndMakeVisible(spinSubdiv);

	btnRewind.setButtonText(UNICON::rewind);
	btnRewind.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
	btnRewind.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
	btnRewind.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::white);
	btnRewind.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::orange);
	addAndMakeVisible(btnRewind);
	btnRewind.onClick = [this]
		{
			this->transportWrapper.rewind_flag = true;
			getPosition();
		};

	btnPlay.setButtonText(UNICON::play);
	btnPlay.setClickingTogglesState(true);
	btnPlay.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
	btnPlay.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
	btnPlay.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::white);
	btnPlay.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::green);
	addAndMakeVisible(btnPlay);

	// connect the play button to the "playing" audio parameter
	attach_Play = transportWrapper.tree.createButtonAttachment(IDS::playing, btnPlay);

	// initialize the playing setup
	setupPlayControl(transportWrapper.host_controls_playing);

}

UI_Transport::~UI_Transport() {}

void UI_Transport::getPosition()
{
	ppq = transportWrapper.getPpq();
	
	// Calculate the number of subdivisions per beat based on beat_duration
	int subDivisionsPerBeat = 16 / transportWrapper.beat_duration;  // This adjusts based on beat duration

	// Calculate the scaler for converting PPQ to beats based on beat duration
	float quarterNotesPerBeat = 4.0f / transportWrapper.beat_duration; // Adjust beat rate based on beat duration

	spinBeats.safeSetRange (1, transportWrapper.bar_length, 1);
	spinSubdiv.safeSetRange (1, (int) subDivisionsPerBeat, 1); // recalculate range

	// convert ppq to number of total beats, based on the beat duration
	float beatPosition = ppq / quarterNotesPerBeat;
	int bars = 1 + ((int)beatPosition / transportWrapper.bar_length);
	int beats = 1 + ((int) ppq % transportWrapper.bar_length);
	int divisions = 1 + ((int) (ppq * subDivisionsPerBeat) % subDivisionsPerBeat);
	
	spinBars.setValue(bars);
	spinBeats.setValue(beats);
	spinSubdiv.setValue(divisions);

	spinBars.timerCallback();
	spinBeats.timerCallback();
	spinSubdiv.timerCallback();
}

void UI_Transport::layout() {
	// get the screen bounds
	auto desktopArea = desktop.getDisplays().getMainDisplay().totalArea; // TODO: use this information to scale the transport view (e.g. for 4k displays)

	auto body = getLocalBounds();
	int p = 5; // padding
	int pp = p * 2; // double padding - added to width and height to account for padding on all sides
	int h = 25; // height of the transport bar
	int w = 450; // width of the transport bar
	int bw = 35; // button width
	
	// define the inner area of the component
	auto area = body.removeFromTop(h + pp).removeFromLeft(getWidth()); // transport should take the full width
	area = area.removeFromLeft(w).reduced(p); // shrink to the width of the transport bar and apply padding

	btnRewind.setBounds(area.removeFromLeft(bw));
	btnPlay.setBounds(area.removeFromLeft(bw));
	
	area.removeFromLeft(bw); // spacer
	
	spinTempoDuration.setBounds(area.removeFromLeft(50));
	spinTempo.setBounds(area.removeFromLeft(50));

	area.removeFromLeft(bw); // spacer

	auto timeSigArea = area.removeFromLeft(64);
	spinBarLength.setBounds(timeSigArea.removeFromLeft(25));
	lblTimeSigSep.setBounds(timeSigArea.removeFromLeft(14));
	spinBeatLength.setBounds(timeSigArea.removeFromLeft(25));

	area.removeFromLeft(bw); // spacer

	auto posArea = area.removeFromLeft(110);
	spinBars.setBounds(posArea.removeFromLeft(50));
	spinBeats.setBounds(posArea.removeFromLeft(30));
	spinSubdiv.setBounds(posArea.removeFromLeft(30));
}

void UI_Transport::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

	g.setColour(juce::Colours::grey);

	g.setColour(juce::Colours::white);

	g.setFont(14.0f);
}

void UI_Transport::resized()
{
	layout();
}

void UI_Transport::setupPlayControl(bool hostControls)
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

/// <summary>
/// Attaches handlers to the transport wrapper or the spinner, depending on the arbiter of the tempo
/// </summary>
/// <param name="hostControls"></param>
void UI_Transport::setupTempo(bool hostControls)
{
	if (hostControls) {
		spinTempo.setEnabled(false);
		// we want to be able to match the host, so we'll allow for 2 decimals of precision
		spinTempo.setNumDecimalPlacesToDisplay(2);
		spinTempo.onValueChange = nullptr;
	}
	else {
		// when there is no host, we can set the tempo to an integer value
		spinTempo.setNumDecimalPlacesToDisplay(0);
		spinTempo.setEnabled(true);
		spinTempo.onValueChange = [&]
			{
				// when the spinner value changes, update the tree
				// this is causing access violation errors
				transportWrapper.tempo = (int)round(spinTempo.getValue()); // BUG: this assignment is causing the listener to trigger, which is sometimes causing an error
			};
	}
}

void UI_Transport::setupTimeSignature(bool hostControls)
{
	if (hostControls) {
		spinBarLength.setEnabled(false);
		spinBeatLength.setEnabled(false);
		spinBarLength.onValueChange = [&] 
			{
				spinBeats.safeSetRange(1, spinBarLength.getValue(), 1);
			};
		spinBeatLength.onValueChange = nullptr;
	}
	else {
		spinBarLength.setEnabled(true);
		spinBeatLength.setEnabled(true);
		spinBarLength.onValueChange = [&]
			{
				transportWrapper.bar_length = spinBarLength.getValue();
			};
		spinBeatLength.onValueChange = [&]
			{
				transportWrapper.beat_duration = spinBeatLength.getValue();
			};
	}
}

void UI_Transport::timerCallback()
{
	spinBarLength.timerCallback();
	spinBeatLength.timerCallback();
	spinTempo.timerCallback();
	
	getPosition();
	// pick up changes from the host
	if (transportWrapper.host_controls_tempo) {
		spinTempo.setValue(transportWrapper.tempo, juce::NotificationType::dontSendNotification); // don't send notification, we're updating in response to a value change - the slider attachment would trigger a second update
	}
	if (transportWrapper.host_controls_time_signature) {
		spinBarLength.setValue(transportWrapper.bar_length, juce::NotificationType::dontSendNotification);
		spinBeatLength.setValue(transportWrapper.beat_duration, juce::NotificationType::dontSendNotification);
	}
}