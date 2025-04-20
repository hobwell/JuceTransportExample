#include "UI_ChoiceSpinner.h"

UI_ChoiceSpinner::UI_ChoiceSpinner(juce::AudioProcessorValueTreeState& apvtsRef,
    const juce::String& paramID_,
    KeyValueList choices)
    : apvts(apvtsRef), paramID(paramID_), keyValues(std::move(choices))
{
    parameter = apvts.getParameter(paramID);
    jassert(parameter != nullptr);

    apvts.addParameterListener(paramID, this);

    updateValueFromParameter();
}

UI_ChoiceSpinner::~UI_ChoiceSpinner()
{
    apvts.removeParameterListener(paramID, this);
}

void UI_ChoiceSpinner::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);

    auto text = keyValues[selectedIndex].first;
    g.drawText(text, getLocalBounds(), juce::Justification::centred, true);
}

void UI_ChoiceSpinner::resized()
{
    // No children to layout
}

void UI_ChoiceSpinner::mouseDown(const juce::MouseEvent& e)
{
    dragStartY = e.y;
}

void UI_ChoiceSpinner::mouseDrag(const juce::MouseEvent& e)
{
    int dragDelta = dragStartY - e.y;
    int steps = dragDelta / 10; // Change value every 10 pixels dragged

    if (steps != 0)
    {
        changeIndex(steps);
        dragStartY = e.y; // Reset to prevent large jumps
    }
}

void UI_ChoiceSpinner::changeIndex(int delta)
{
    setIndex(juce::jlimit(0, static_cast<int>(keyValues.size()) - 1, selectedIndex + delta));
}

void UI_ChoiceSpinner::setIndex(int newIndex)
{
    if (newIndex != selectedIndex && parameter != nullptr)
    {
        selectedIndex = newIndex;
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost(parameter->convertTo0to1(keyValues[selectedIndex].second));
        parameter->endChangeGesture();
        DBG("getValue() " << parameter->getValue());
        DBG("convertTo0to1() " << parameter->convertTo0to1(parameter->getValue()));
        DBG("convertFrom0to1() " << parameter->convertFrom0to1(parameter->getValue()));
        repaint();
    }
}

void UI_ChoiceSpinner::syncWithParameterValue(float value)
{
    for (size_t i = 0; i < keyValues.size(); ++i)
    {
        if (juce::approximatelyEqual(keyValues[i].second, value))
        {
            selectedIndex = static_cast<int>(i);
            repaint();
            return;
        }
    }
}

void UI_ChoiceSpinner::updateValueFromParameter()
{
    if (parameter != nullptr)
    {
        float actualValue = parameter->convertFrom0to1(parameter->getValue());
        syncWithParameterValue(actualValue);
    }
}

void UI_ChoiceSpinner::parameterChanged(const juce::String& id, float newValue)
{
    if (id == paramID)
    {
        // Called from a non-UI thread; must sync with the UI thread
        juce::MessageManager::callAsync([this, newValue] ()
            {
                syncWithParameterValue(parameter->convertFrom0to1(newValue));
            });
    }
}