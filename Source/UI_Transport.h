/*
  ==============================================================================

    UI_Transport.h
    Created: 14 Sep 2024 1:08:24pm
    Author:  Nicholas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "UI_Spinner.h"
#include "UI_ChoiceSpinner.h"
#include "TransportParameters.h"
#include "FontAwesome_LookAndFeel.h"
#include "UI_TransportTimeline.h"

/// <summary>
/// Basic playback, tempo and time signature and position control - disabled when controlled by a host
/// </summary>
class UI_Transport : public juce::Component, public juce::ChangeBroadcaster
{
public:
    bool playing = false;

    UI_Transport(TransportParameters& transportParams);
    ~UI_Transport() override;

    void updatePpqFromUI(std::function<void(float)> f);

    void paint(juce::Graphics&) override;
    void resized() override;
private:
    FontAwesome_LookAndFeel fontAwesome;

    bool initialized = false;
    float ppq = 0.f;

    juce::Label lblTimeSigSep;
    UI_ChoiceSpinner spinTempoDuration;
    UI_Spinner spinBarLength{ 0, juce::Justification::centred, false };
    UI_Spinner spinBeatLength{ 0, juce::Justification::centred, false};
    UI_Spinner spinBars{ 0, juce::Justification::right, true };
    UI_Spinner spinBeats{ 0, juce::Justification::right, true };
    UI_Spinner spinBeatDivisions{ 0, juce::Justification::right, false };
    UI_Spinner spinTempo{ 2, juce::Justification::centred, true };
    UI_TransportTimeline transportTimeline;

    TransportParameters& transportParams;

    juce::Desktop& desktop{ juce::Desktop::getInstance() };

    void layout();

    void setupPlayControl(bool hostControls);
    void setupPosition(bool hostControls);
    void setupTempo(bool hostControls);
    void setupTempoRelativeNoteDuration(bool hostControls);
    void setupTimeSignature(bool hostControls);

    juce::TextButton btnPlay;
    juce::TextButton btnRewind;

    // attachments need to be created where they will be destroyed before the components they attache to
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attach_Tempo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attach_TempoOptions;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attach_BarLength;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attach_BeatLength;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attach_Pos_Bar;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attach_Pos_Beat;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attach_Pos_Div;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attach_Play;
    std::unique_ptr<GenericComponentAttachment> attach_Timeline_playing;
    std::unique_ptr<GenericComponentAttachment> attach_Timeline_ppq;
    std::unique_ptr<GenericComponentAttachment> attach_Timeline_barLength;
    std::unique_ptr<GenericComponentAttachment> attach_Timeline_beatDuration;
    std::unique_ptr<GenericComponentAttachment> attach_Timeline_bar;
    std::unique_ptr<GenericComponentAttachment> attach_Timeline_beat;
    std::unique_ptr<GenericComponentAttachment> attach_Timeline_division;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UI_Transport)
};
