/*
  ==============================================================================

    GenericComponentAttachment.h
    Created: 3 May 2025 11:56:11am
    Author:  Nicholas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class UI_TransportTimeline : 
    public juce::Component, 
    private juce::Timer
{
public:
    UI_TransportTimeline();
    ~UI_TransportTimeline() override;

    void setBarLength(int);
    
    void setBeatDuration(int);

    void setTransportPosition(float newBarPosition);

    void setPlaying(bool playing);

    void zoom(float zoomFactor);

    void pan(float deltaBars);

    void paint(juce::Graphics& g) override;
    
private:
    void timerCallback() override;

    int barLength = 4;
    int beatDuration = 4;
    float transportBarPosition = 0.0f;
    float centerBar = 4.0f; // start around bar 5
    float visibleBarCount = 8.0f;
    bool isPlaying = false;
};
