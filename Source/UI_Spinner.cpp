/*
  ==============================================================================

    UI_Spinner.cpp
    Created: 14 Sep 2024 1:23:17pm
    Author:  Nicholas

  ==============================================================================
*/

#include <JuceHeader.h>
#include "UI_Spinner.h"

UI_Spinner::UI_Spinner(int numDecimalsToDisplay, juce::Justification align = juce::Justification::centred, bool alwaysShowDecimal = false) : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::NoTextBox)
{
    setNumDecimalPlacesToDisplay(numDecimalsToDisplay);
    permanentDecimal = alwaysShowDecimal;

    label.setText(getDisplayString(), juce::NotificationType::dontSendNotification);
    label.setInterceptsMouseClicks(false, false);
    label.setJustificationType(align);
    addAndMakeVisible(label);
    /*onValueChange = [this] ()
        {
            DBG("UI_Spinner value changed: " << getValue());
        };*/
}

UI_Spinner::~UI_Spinner() {}

/// <summary>
/// Set the number of decimal places to display in the box
/// </summary>
/// <param name="numDecimals"></param>
/// <remarks>
/// Because the slider may be linked to an audio parameter, we need a method to set the precision of the text being displayed 
/// there are overloads for the parameter to provide a string converter for it, but we can't know what conversion function we 
/// want (based on the transport) until after the parameters have been initialized, so we'll use a local function to handle it.
/// </remarks>
juce::String UI_Spinner::getDisplayString()
{
    std::stringstream ss;

    ss << std::fixed << std::setprecision(getNumDecimalPlacesToDisplay()) << getValue();
    if (permanentDecimal && getNumDecimalPlacesToDisplay() == 0)
    {
        ss << ".";
    }

    std::string stringValue = ss.str();
    return stringValue;
}

void UI_Spinner::mouseDown(const juce::MouseEvent& e)
{
    juce::Slider::mouseDown(e);
}

void UI_Spinner::mouseUp(const juce::MouseEvent& e)
{
    juce::Slider::mouseUp(e);
}

void UI_Spinner::paint(juce::Graphics& g)
{
    // don't draw anything
}

void UI_Spinner::resized()
{
    juce::Slider::resized();
    label.setBounds(getLocalBounds());
}

void UI_Spinner::safeSetRange(double min, double max, double interval)
{
    if (min == max)
    {
        max = min + 1;
        setValue(min);
        setLocked(true);
    }
    else
    {
        setLocked(false);
    }

    Slider::setRange(min, max, interval);
}

void UI_Spinner::setLocked(bool locked)
{
    this->wasEnabled = isEnabled() == true;
    this->locked = locked;
    if (wasEnabled)
    {
        if (locked)
        {
            setEnabled(false);
        }
        else
        {
            setEnabled(true);
        }
    }
    else
    {
        setEnabled(false);
    }
}

void UI_Spinner::valueChanged()
{
    label.setText(getDisplayString(), juce::NotificationType::dontSendNotification);
    juce::NullCheckedInvocation::invoke(onValueChanged, getValue());
}