/*
  ==============================================================================

    Spinner_UI.h
    Created: 14 Sep 2024 1:23:17pm
    Author:  Nicholas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TransportParameters.h"
#include "Spinner_LookAndFeel.h"

/// <summary>
/// A MAX style number box that allows the user to click and drag to change the value of a parameter.
/// </summary>
// TODO: Make a better spinner...
class UI_Spinner : 
    public juce::Slider
{
public:
    UI_Spinner(int numDecimalsToDisplay, juce::Justification align, bool alwaysShowDecimal);
    ~UI_Spinner() override;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void paint(juce::Graphics&) override;
    void resized() override;
    void valueChanged() override;
    std::function<void()> onUserEditComplete; // fired when the user has finished editing the value via the text box or spinner drag
    std::function<void(int)> onValueChanged;

    void safeSetRange(double newMin, double newMax, double newInterval);
    void setAttachedParameter(juce::RangedAudioParameter*);
private:
    juce::AudioParameterFloat* attachedParameter = nullptr;
    int displayPrecision = 0; // number of decimal places to display
    bool wasDragging = false;
    int dragStartY = 0;
    juce::Time lastDragTime;
    int lastDragY = 0;
    bool isAdjustingDecimal;
    float initialValue = 0.0f;
    juce::Label label;
    bool permanentDecimal = false; // indicates whether the decimal should always be shown (even when there are no decimlal places being displayed)
    Spinner_LookAndFeel lookAndFeel;
    bool wasEnabled = true; // to handle when the range is a single value - we need to enhance isEnabled() with this
    juce::NotificationType waitingNotificationType = juce::NotificationType::dontSendNotification;

    juce::String getDisplayString();
    juce::String getTextFromValue(double value) override;
    void setLocked(bool locked);
    bool valueIsInRange(float value);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UI_Spinner)
};