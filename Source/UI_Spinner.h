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
// TODO: Make a better spinner...
class UI_Spinner : public juce::Slider
{
public:
    UI_Spinner(int numDecimalsToDisplay, juce::Justification align, bool alwaysShowDecimal);
    ~UI_Spinner() override;

    bool permanentDecimal = false; // indicates whether the decimal should always be shown (even when there are no decimlal places being displayed)

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void paint(juce::Graphics&) override;
    void resized() override;
    //void setValue(float newValue, juce::NotificationType notification = juce::NotificationType::sendNotificationAsync);
    void valueChanged() override;

    void safeSetRange(double newMin, double newMax, double newInterval);

    std::function<void(int)> onValueChanged;

private:
    juce::Label label;
    bool locked = false; // to handle when the range is a single value - we need to enhance isEnabled() with this
    bool wasEnabled = true; // to handle when the range is a single value - we need to enhance isEnabled() with this
    juce::NotificationType waitingNotificationType = juce::NotificationType::dontSendNotification;

    juce::String getDisplayString();

    void setLocked(bool locked);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UI_Spinner)
};
