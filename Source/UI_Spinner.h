/*
  ==============================================================================

    Spinner_UI.h
    Created: 14 Sep 2024 1:23:17pm
    Author:  Nicholas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TransportTree.h"

/// <summary>
/// A MAX style number box that allows the user to click and drag to change the value of a parameter.
/// </summary>
class UI_Spinner : public juce::Slider
{
public:
    UI_Spinner(int numDecimalsToDisplay);
    ~UI_Spinner() override;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void paint(juce::Graphics&) override;
    void resized() override;
    void setValue(float newValue, juce::NotificationType notification = juce::NotificationType::sendNotificationAsync);
    void timerCallback();
    void valueChanged() override;

    std::function<void(int)> onValueChanged;

private:
    std::atomic<bool> awaitingChange = false;
    juce::Label label;
    juce::NotificationType waitingNotificationType = juce::NotificationType::dontSendNotification;
    float waitingValue = 0.f;
    
    void updateGui();
    juce::String getDisplayString(const double value);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UI_Spinner)
};
