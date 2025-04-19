/*
  ==============================================================================

    UI_Spinner.cpp
    Created: 14 Sep 2024 1:23:17pm
    Author:  Nicholas

  ==============================================================================
*/

#include <JuceHeader.h>
#include "UI_Spinner.h"
#include "KeyValueParameter.h"

UI_Spinner::UI_Spinner(int numDecimalsToDisplay, juce::Justification align = juce::Justification::centred, bool alwaysShowDecimal = false) : juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::NoTextBox)
{
    setNumDecimalPlacesToDisplay(numDecimalsToDisplay);
    permanentDecimal = alwaysShowDecimal;

    label.setText(getDisplayString(getValue()), juce::NotificationType::dontSendNotification);
    label.setInterceptsMouseClicks(false, false);
    label.setJustificationType(align);
    addAndMakeVisible(label);
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
juce::String UI_Spinner::getDisplayString(const double value)
{
    std::stringstream ss;
    if (keyValueMap != nullptr)
    {
        int index = juce::jlimit(0, (int) keyValueMap->size() - 1, (int) std::round(value));
        return (*keyValueMap)[index].key;
    }

    ss << std::fixed << std::setprecision(getNumDecimalPlacesToDisplay()) << value;
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
        setEnabled(false);
    }
    else
    {
        setEnabled(true);
    }

    Slider::setRange(min, max, interval);
}

void UI_Spinner::setValueMap(const std::vector<KeyValuePair>* map)
{
    keyValueMap = map;
    for (size_t i = 0; i < keyValueMap->size(); ++i)
    {
        labelToIndex[(*keyValueMap)[i].key] = static_cast<int>(i);
    }
}

void UI_Spinner::setValue(float newValue, juce::NotificationType notificationType)
{
    waitingNotificationType = notificationType;
    waitingValue = newValue;
    awaitingChange = true;
}

void UI_Spinner::setValueFromKey(const std::string& key)
{
    if (keyValueMap != nullptr)
    {
        auto it = std::find_if(keyValueMap->begin(), keyValueMap->end(),
            [&key](const KeyValuePair& pair) { return pair.key == key; });

        if (it != keyValueMap->end())
        {
            // Set the corresponding value in the APVTS (you can replace `setValue` with APVTS update logic)
            setValue((*it).value);
        }
    }
}
void UI_Spinner::timerCallback()
{
    updateGui();
}

void UI_Spinner::updateGui()
{
    if (awaitingChange)
    {
        juce::Slider::setValue(waitingValue, waitingNotificationType);
        label.setText(getDisplayString(getValue()), waitingNotificationType);
        awaitingChange = false;
    }
}

void UI_Spinner::valueChanged()
{
    label.setText(getDisplayString(getValue()), juce::NotificationType::dontSendNotification);
    juce::NullCheckedInvocation::invoke(onValueChanged, getValue());
}