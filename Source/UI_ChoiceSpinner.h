#pragma once

#include <JuceHeader.h>

class UI_ChoiceSpinner : public juce::Slider,
    private juce::AudioProcessorValueTreeState::Listener
{
public:
    using KeyValueList = std::vector<std::pair<juce::String, float>>;

    UI_ChoiceSpinner(juce::AudioProcessorValueTreeState& apvts,
        const juce::String& paramID,
        KeyValueList choices);

    ~UI_ChoiceSpinner() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;

private:
    void updateValueFromParameter();
    void changeIndex(int delta);
    void setIndex(int newIndex);
    void syncWithParameterValue(float value);
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    juce::AudioProcessorValueTreeState& apvts;
    juce::RangedAudioParameter* parameter = nullptr;
    juce::String paramID;

    KeyValueList keyValues;
    int selectedIndex = 0;

    int dragStartY = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UI_ChoiceSpinner)
};