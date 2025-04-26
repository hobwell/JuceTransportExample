#pragma once

#include <JuceHeader.h>

class UI_ChoiceSpinner : public juce::Slider
{
public:
    using KeyValueList = std::vector<std::pair<juce::String, float>>;

    UI_ChoiceSpinner(KeyValueList choices);
    ~UI_ChoiceSpinner() override = default;

    
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void valueChanged() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    std::function<void(int)> onValueChanged;
private:
    int dragStartY = 0;
    float initialValue = 0.0f;
    KeyValueList keyValues;
    juce::Label label;

    float getClosestValue(float value) const;
    juce::String getDisplayString() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UI_ChoiceSpinner)
};