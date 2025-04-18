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
#include "TransportTree.h"
#include "APVTSWrapper.h"

/// <summary>
/// Basic playback, tempo and time signature and position control - disabled when controlled by a host
/// </summary>
class UI_Transport : public juce::Component, public juce::ChangeBroadcaster
{
public:
    bool playing = false;

    UI_Transport(APVTSWrapper& transportWrapper);
    ~UI_Transport() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback();
private:
    static const juce::String stopIcon;
    static const juce::String playIcon;
    static const juce::String rewindIcon;

    bool initialized = false;
    float ppq = 0.f;

    juce::Label lblTimeSigSep;
    UI_Spinner spinBarLength{ 0, juce::Justification::centred, false };
    UI_Spinner spinBeatLength{ 0, juce::Justification::centred, false};
    UI_Spinner spinBars{ 0, juce::Justification::right, true };
    UI_Spinner spinBeats{ 0, juce::Justification::right, true };
    UI_Spinner spinSubdiv{ 0, juce::Justification::right, false };
    UI_Spinner spinTempo{ 0, juce::Justification::centred, false };
    APVTSWrapper& transportWrapper;

    juce::Desktop& desktop{ juce::Desktop::getInstance() };

    void layout();

    void setupPlayControl(bool hostControls);
    void setupTempo(bool hostControls);
    void setupTimeSignature(bool hostControls);

    void getPosition();

    juce::TextButton btnPlay;
    juce::TextButton btnRewind;

    // attachments need to be created where they will be destroyed before the components they attache to
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attach_Tempo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attach_BarLength;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attach_BeatLength;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attach_Play;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UI_Transport)
};
