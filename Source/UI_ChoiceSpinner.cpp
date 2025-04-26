#include "UI_ChoiceSpinner.h"

UI_ChoiceSpinner::UI_ChoiceSpinner(KeyValueList choices) : 
    juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::NoTextBox),
    keyValues(std::move(choices))
{
    // Label for displaying selected choice
    label.setText(getDisplayString(), juce::NotificationType::dontSendNotification);
    label.setInterceptsMouseClicks(false, false);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);

    onValueChange = [this] ()
        {
            DBG("UI_ChoiceSpinner value changed: " << getValue());
        };
}

void UI_ChoiceSpinner::mouseDown(const juce::MouseEvent& e)
{
    dragStartY = e.y;
    initialValue = getValue();
    juce::Slider::mouseDown(e);
}

void UI_ChoiceSpinner::mouseDrag(const juce::MouseEvent& e)
{
    if (!isEnabled()) return;
    int dragDelta = dragStartY - e.y;
    int step = dragDelta / 10; // 10 pixels per step
    int currentIndex = 0;

    for (size_t i = 0; i < keyValues.size(); ++i)
    {
        if (std::abs(keyValues[i].second - initialValue) < 0.0001f)
        {
            currentIndex = static_cast<int>(i);
            break;
        }
    }

    int newIndex = juce::jlimit(0, static_cast<int>(keyValues.size()) - 1, currentIndex + step);
    float newValue = keyValues[newIndex].second;
    setValue(newValue, juce::sendNotificationAsync); // this triggers attachment update
}

float UI_ChoiceSpinner::getClosestValue(float value) const
{
    float closest = keyValues.front().second;
    float minDiff = std::abs(value - closest);

    for (const auto& kv : keyValues)
    {
        float diff = std::abs(value - kv.second);
        if (diff < minDiff)
        {
            closest = kv.second;
            minDiff = diff;
        }
    }

    return closest;
}

juce::String UI_ChoiceSpinner::getDisplayString() const
{
    float closest = getClosestValue(getValue());
    for (const auto& kv : keyValues)
    {
        if (std::abs(kv.second - closest) < 0.0001f)
            return kv.first;
    }
    return {};
}

void UI_ChoiceSpinner::paint(juce::Graphics& g)
{
    // do nothing
}

void UI_ChoiceSpinner::resized()
{
    juce::Slider::resized();
    label.setBounds(getLocalBounds());
}

void UI_ChoiceSpinner::valueChanged()
{
    // Update label text when value changes
    label.setText(getDisplayString(), juce::NotificationType::dontSendNotification);
    juce::NullCheckedInvocation::invoke(onValueChanged, getValue());
}