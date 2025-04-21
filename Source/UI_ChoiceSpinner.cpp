#include "UI_ChoiceSpinner.h"

UI_ChoiceSpinner::UI_ChoiceSpinner(KeyValueList choices)
    : keyValues(std::move(choices))
{
    setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    onValueChange = [this] ()
        {
            DBG("UI_ChoiceSpinner value changed: " << getValue());
        };
}

void UI_ChoiceSpinner::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack);

    auto label = getLabelForValue(getValue());
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawFittedText(label, getLocalBounds(), juce::Justification::centred, 1);
}

void UI_ChoiceSpinner::mouseDown(const juce::MouseEvent& e)
{
    dragStartY = e.y;
    initialValue = getValue();
}

void UI_ChoiceSpinner::mouseDrag(const juce::MouseEvent& e)
{
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

juce::String UI_ChoiceSpinner::getLabelForValue(float value) const
{
    float closest = getClosestValue(value);
    for (const auto& kv : keyValues)
    {
        if (std::abs(kv.second - closest) < 0.0001f)
            return kv.first;
    }
    return {};
}

void UI_ChoiceSpinner::valueChanged()
{
    juce::NullCheckedInvocation::invoke(onValueChanged, getValue());
}