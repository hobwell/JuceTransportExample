/*
  ==============================================================================

    UI_Timeline.cpp
    Created: 3 May 2025 11:49:48am
    Author:  Nicholas

  ==============================================================================
*/

#include "UI_TransportTimeline.h"
#include "TransportLayoutConstants.h"

UI_TransportTimeline::UI_TransportTimeline()
{
    startTimerHz(60); // update at 60 fps
}

UI_TransportTimeline::~UI_TransportTimeline()
{
    stopTimer();
}

void UI_TransportTimeline::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    const float pixelsPerBar = getWidth() / visibleBarCount;
    const float startBar = centerBar - (visibleBarCount * 0.5f);
    const int totalBars = (int) std::ceil(visibleBarCount) + 1;

    for (int i = 0; i < totalBars; ++i)
    {
        float bar = std::floor(startBar) + i;
        if (bar < 0)
            continue;

        float x = (bar - startBar) * pixelsPerBar;

        // Draw beat divisions
        for (int b = 1; b < barLength; ++b)
        {
            float beatOffset = b / static_cast<float>(barLength);
            float beatX = x + beatOffset * pixelsPerBar;

            g.setColour(juce::Colours::lightgrey);
            g.drawLine(beatX, 0.0f, beatX, (float) getHeight(), 1.0f);
        }

        // Draw bar line
        g.setColour(bar == std::floor(transportBarPosition) ? juce::Colours::orange : juce::Colours::white);
        g.drawLine(x, 0.0f, x, (float) getHeight(), 1.5f);

        // Draw bar label
        g.setColour(juce::Colours::white);
        g.drawText(juce::String((int) bar + 1), (int) x + 4, 0, 60, 16, juce::Justification::left);
    }

    // Draw playhead in center
    g.setColour(juce::Colours::red);
    g.drawLine((float) getWidth() / 2, 0.0f, (float) getWidth() / 2, (float) getHeight(), 2.0f);
}



void UI_TransportTimeline::pan(float deltaBars)
{
    if (!isPlaying)
    {
        centerBar += deltaBars;
        repaint();
    }
}

void UI_TransportTimeline::setBarLength(int barLength)
{
    this->barLength = barLength;
    repaint();
}

void UI_TransportTimeline::setBeatDuration(int beatDuration)
{
    this->beatDuration = beatDuration;
    repaint();
}

void UI_TransportTimeline::setPlaying(bool playing)
{
    isPlaying = playing;
}

void UI_TransportTimeline::setTransportPosition(float newPpqPosition)
{
    // Calculate the number of subdivisions per beat based on beat_duration
    int subDivisionsPerBeat = 16 / beatDuration;  // This adjusts based on beat duration

    // Calculate the scaler for converting PPQ to beats based on beat duration
    float quarterNotesPerBeat = 4.0f / beatDuration; // Adjust beat rate based on beat duration

    // convert ppq to number of total beats, based on the beat duration
    float beatPosition = newPpqPosition / quarterNotesPerBeat;

    transportBarPosition = (beatPosition / barLength);
    centerBar = transportBarPosition;

    repaint();
}

void UI_TransportTimeline::timerCallback()
{
    if (isPlaying)
        repaint();
}

void UI_TransportTimeline::zoom(float zoomFactor)
{
    visibleBarCount = juce::jlimit(1.0f, 32.0f, visibleBarCount * zoomFactor);
    repaint();
}