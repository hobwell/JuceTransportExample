/*
  ==============================================================================

    UI_Transport.cpp
    Created: 14 Sep 2024 1:08:24pm
    Author:  Nicholas

  ==============================================================================
*/

#include <JuceHeader.h>
#include "UI_Transport.h"
#include "TransportConstants.h"

//==============================================================================
UI_Transport::UI_Transport(TransportParameters& transportParams) :
    transportParams(transportParams),
    spinTempoDuration(TRANSPORT::TEMPO::duration_options)
{
    // connect sliders to audio parameters
    // this will adjust the slider's range to match the parameter's range
    attach_Tempo = transportParams.createSliderAttachment(TRANSPORT::IDS::tempo, spinTempo);
    // let the slider know that it is attached to a parameter - needed for range checking
    spinTempo.setAttachedParameter(transportParams.apvts.getParameter(TRANSPORT::IDS::tempo));
    // make control visible
    addAndMakeVisible(spinTempo);

    attach_TempoOptions = transportParams.createSliderAttachment(TRANSPORT::IDS::tempo_speed, spinTempoDuration);
    addAndMakeVisible(spinTempoDuration);

    attach_BarLength = transportParams.createSliderAttachment(TRANSPORT::IDS::bar_length, spinBarLength);
    // let the slider know that it is attached to a parameter - needed for range checking
    spinBarLength.setAttachedParameter(transportParams.apvts.getParameter(TRANSPORT::IDS::bar_length));
    addAndMakeVisible(spinBarLength);

    lblTimeSigSep.setText("/", juce::dontSendNotification);
    addAndMakeVisible(lblTimeSigSep);

    attach_BeatLength = transportParams.createSliderAttachment(TRANSPORT::IDS::beat_duration, spinBeatLength);
    // let the slider know that it is attached to a parameter - needed for range checking
    spinBeatLength.setAttachedParameter(transportParams.apvts.getParameter(TRANSPORT::IDS::beat_duration));
    addAndMakeVisible(spinBeatLength);

    attach_Pos_Bar = transportParams.createSliderAttachment(TRANSPORT::IDS::pos_bar, spinBars);
    // let the slider know that it is attached to a parameter - needed for range checking
    spinBars.setAttachedParameter(transportParams.apvts.getParameter(TRANSPORT::IDS::pos_bar));
    addAndMakeVisible(spinBars);

    attach_Pos_Beat = transportParams.createSliderAttachment(TRANSPORT::IDS::pos_beat, spinBeats);
    // let the slider know that it is attached to a parameter - needed for range checking
    spinBeats.setAttachedParameter(transportParams.apvts.getParameter(TRANSPORT::IDS::pos_beat));
    addAndMakeVisible(spinBeats);

    attach_Pos_Div = transportParams.createSliderAttachment(TRANSPORT::IDS::pos_div, spinBeatDivisions);
    // let the slider know that it is attached to a parameter - needed for range checking
    spinBeatDivisions.setAttachedParameter(transportParams.apvts.getParameter(TRANSPORT::IDS::pos_div));
    addAndMakeVisible(spinBeatDivisions);

    attach_Timeline_ppq = transportParams.createGenericAttachment(
        TRANSPORT::IDS::ppq,
        transportTimeline,
        [&] (float ppq) { transportTimeline.setTransportPosition(ppq); },
        [&] (std::function<void(float)> f) { 
            /* TODO: when the timeline is repositioned (via dragging), update ppq */ 
        }
    );
    attach_Timeline_playing = transportParams.createGenericAttachment(
        TRANSPORT::IDS::playing,
        transportTimeline,
        [&] (bool playing) 
        { 
            transportTimeline.setPlaying(playing);
            setupPosition(*transportParams.host_controls_playing || playing);
        },
        [] (std::function<void(float)>) {}  // Explicitly does nothing
    );
    attach_Timeline_barLength = transportParams.createGenericAttachment(
        TRANSPORT::IDS::bar_length,
        transportTimeline,
        [&] (int barLength) { transportTimeline.setBarLength(barLength); },
        [] (std::function<void(float)>) {}  // Explicitly does nothing
    );
    attach_Timeline_beatDuration = transportParams.createGenericAttachment(
        TRANSPORT::IDS::beat_duration,
        transportTimeline,
        [&] (int beatDuration) 
        { 
            transportTimeline.setBeatDuration(beatDuration); 
        },
        [&] (std::function<void(float)> f)
        {
            spinBeatLength.onDragEnd = [&, f]
                {
                    // update the position when the beat duration is changed
                    ppq = transportParams.getPpq();
                    transportTimeline.setTransportPosition(ppq);
                    transportParams.setPos(ppq, true);
                };
            spinBeatLength.onUserEditComplete = [&, f]
                {
                    // update the position when the beat duration is changed
                    ppq = transportParams.getPpq();
                    transportTimeline.setTransportPosition(ppq);
                    transportParams.setPos(ppq, true);
                };
        }
    );
    attach_Timeline_bar = transportParams.createGenericAttachment(
        TRANSPORT::IDS::pos_bar,
        spinBars,
        [&] (float bar) {}, // Explicitly does nothing
        [&] (std::function<void(float)> f)
        {
            // update the ppq when the bar is changed
            spinBars.onUserEditComplete = [&, f] { updatePpqFromUI(f); };
        }
    );
    attach_Timeline_beat = transportParams.createGenericAttachment(
        TRANSPORT::IDS::pos_beat,
        spinBeats,
        [&] (float beat) {}, // Explicitly does nothing
        [&] (std::function<void(float)> f)
        {
            // update the ppq when the bar is changed
            spinBeats.onUserEditComplete = [&, f] { updatePpqFromUI(f); };
        }
    );
    attach_Timeline_division = transportParams.createGenericAttachment(
        TRANSPORT::IDS::pos_div,
        spinBeatDivisions,
        [&] (float div) {}, // Explicitly does nothing
        [&] (std::function<void(float)> f)
        {
            // update the ppq when the bar is changed
            spinBeatDivisions.onUserEditComplete = [&, f] { updatePpqFromUI(f); };
        }
    );
    addAndMakeVisible(transportTimeline);

    // setup handlers, after all parameters have been attached

    // if the arbiter of the tempo changes, re-initialize the tempo setup
    setupTempo(*transportParams.host_controls_tempo);
    setupTempoRelativeNoteDuration(*transportParams.host_controls_tempo_speed);
    setupTimeSignature(*transportParams.host_controls_time_signature);
    setupPosition(*transportParams.host_controls_position);

    btnRewind.setLookAndFeel(&fontAwesome);
    btnRewind.setButtonText(fontAwesome.icon_backward);
    btnRewind.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
    btnRewind.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
    btnRewind.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::white);
    btnRewind.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::orange);
    addAndMakeVisible(btnRewind);
    btnRewind.onClick = [this]
        {
            *this->transportParams.reposition_flag = true;
            this->transportParams.setPpq(0.f);
        };

    // connect the play button to the "playing" audio parameter
    attach_Play = transportParams.createButtonAttachment(TRANSPORT::IDS::playing, btnPlay);

    btnPlay.setLookAndFeel(&fontAwesome);
    btnPlay.setButtonText(fontAwesome.icon_play);
    btnPlay.setClickingTogglesState(true);
    btnPlay.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::green);
    btnPlay.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::white);
    btnPlay.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::white);
    btnPlay.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::green);
    addAndMakeVisible(btnPlay);

    // initialize the playing setup
    setupPlayControl(*transportParams.host_controls_playing);

}

UI_Transport::~UI_Transport() {}

void UI_Transport::layout()
{
    // get the screen bounds
    auto desktopArea = desktop.getDisplays().getMainDisplay().totalArea; // TODO: use this information to scale the transport view (e.g. for 4k displays)

    auto body = getLocalBounds();
    int p = TRANSPORT::LAYOUT::PADDING; // padding
    int pp = p * 2; // double padding - added to width and height to account for padding on all sides
    int h = TRANSPORT::LAYOUT::HEIGHT;
    int w = body.getWidth();
    int bw = TRANSPORT::LAYOUT::BUTTON::WIDTH;

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
    spinBeatDivisions.setBounds(posArea.removeFromLeft(30));

    transportTimeline.setBounds(body.removeFromTop(TRANSPORT::LAYOUT::TIMELINE::HEIGHT).reduced(p));
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
            if (btnPlay.getToggleState())
            {
                btnPlay.setButtonText(fontAwesome.icon_stop);
            }
            else
            {
                btnPlay.setButtonText(fontAwesome.icon_play);
            }
            if (!this->transportParams.host_controls_playing)
            {
                // also set the transport playing state
                *this->transportParams.playing = btnPlay.getToggleState();
            }
        };
}

void UI_Transport::setupPosition(bool hostControls)
{
    spinBars.setEnabled(!hostControls);
    spinBeats.setEnabled(!hostControls);
    spinBeatDivisions.setEnabled(!hostControls);
}


/// <summary>
/// Attaches handlers to the transport wrapper or the spinner, depending on the arbiter of the tempo
/// </summary>
/// <param name="hostControls"></param>
void UI_Transport::setupTempo(bool hostControls)
{
    if (hostControls)
    {
        spinTempo.setEnabled(false);
        // we want to be able to match the host, so we'll allow for 2 decimals of precision
        spinTempo.setNumDecimalPlacesToDisplay(2);
        spinTempo.onValueChange = nullptr;
    }
    else
    {
        // when there is no host, we can set the tempo to an integer value
        spinTempo.setNumDecimalPlacesToDisplay(0);
        spinTempo.setEnabled(true);
        //spinTempo.onValueChange = [&]
        //	{
        //		// when the spinner value changes, update the tree
        //		// this is causing access violation errors
        //		transportParams.tempo = (int)round(spinTempo.getValue()); // BUG: this assignment is causing the listener to trigger, which is sometimes causing an error
        //	};
    }
}

/// <summary>
/// Attaches handlers to the transport wrapper or the spinner, depending on the arbiter of the tempo relative note duration
/// </summary>
/// <param name="hostControls"></param>
void UI_Transport::setupTempoRelativeNoteDuration(bool hostControls)
{
    if (hostControls)
    {
        spinTempoDuration.setEnabled(false);
        spinTempoDuration.onValueChange = nullptr;
    }
    else
    {
        // when there is no host, we can set the tempo to an integer value
        spinTempoDuration.setEnabled(true);
        //spinTempoDuration.onValueChange = [&]
        //	{
        //		// when the spinner value changes, update the tree
        //		transportParams.tempo_speed = spinTempoDuration.getValue();
        //	};
    }
}

void UI_Transport::setupTimeSignature(bool hostControls)
{
    spinBarLength.onValueChange = [&]
        {
            // safely allow beats (i.e. transport position (bars:beats:divs) to be a range of 1 value (i.e. if there is 1 beat per bar)
            spinBeats.safeSetRange(1, spinBarLength.getValue(), 1);
        };

    spinBeatLength.onValueChange = [&]
        {
            // safely allow divisions (i.e. transport position (bars:beats:divs) to be a range of 1 value (i.e. if there is 1 beat division per beat)
            spinBeatDivisions.safeSetRange(1, 16 / spinBeatLength.getValue(), 1);
        };

    if (hostControls)
    {
        spinBarLength.setEnabled(false);
        spinBeatLength.setEnabled(false);
        spinBeatLength.onValueChange = nullptr;
    }
    else
    {
        spinBeats.safeSetRange(1, spinBarLength.getValue(), 1);
        spinBeatDivisions.safeSetRange(1, 16 / spinBeatLength.getValue(), 1);

        spinBarLength.setEnabled(true);
        spinBeatLength.setEnabled(true);
    }
}

void UI_Transport::updatePpqFromUI(std::function<void(float)> f)
{
    int subDivisionsPerBeat = 16 / *transportParams.beat_duration;
    float quarterNotesPerBeat = 4.0f / *transportParams.beat_duration;

    int barIndex = spinBars.getValue() - 1;
    int beatIndex = spinBeats.getValue() - 1;
    int divIndex = spinBeatDivisions.getValue() - 1;

    float totalBeats = barIndex * *transportParams.bar_length + beatIndex + (float) divIndex / subDivisionsPerBeat;
    ppq = totalBeats * quarterNotesPerBeat;

    DBG(ppq);
    *transportParams.reposition_flag = true;
    transportParams.setPpq(ppq);
}