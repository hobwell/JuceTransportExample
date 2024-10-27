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
    UI_Spinner tempoSpinner{ 0 };
    APVTSWrapper& transportWrapper;

    void tempoSetup(bool hostControls);

    juce::String getPosition();

    juce::Label transportPositionLabel;

    juce::TextButton btnPlay;
    juce::TextButton btnRewind;

    // attachments need to be created where they will be destroyed before the components they attache to
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tempoAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UI_Transport)
};
