/*
  ==============================================================================

    UI_Spinner.cpp
    Created: 14 Sep 2024 1:23:17pm
    Author:  Nicholas

  ==============================================================================
*/

#include <JuceHeader.h>
#include "UI_Spinner.h"

UI_Spinner::UI_Spinner(int numDecimalsToDisplay, juce::Justification align = juce::Justification::centred, bool alwaysShowDecimal = false) :
    juce::Slider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::NoTextBox),
    displayPrecision(numDecimalsToDisplay),
    permanentDecimal(alwaysShowDecimal)
{
    label.setLookAndFeel(&lookAndFeel.inactive);
    label.setText(getDisplayString(), juce::NotificationType::dontSendNotification);
    label.setInterceptsMouseClicks(false, false);
    label.setJustificationType(align);
    label.onTextChange = [this] ()
        {
            double newValue = label.getText().getDoubleValue();
            if (valueIsInRange(newValue))
            {
                setValue(newValue, juce::sendNotification);
            }
            label.setText(getDisplayString(), juce::NotificationType::dontSendNotification);
            juce::NullCheckedInvocation::invoke(onUserEditComplete);
        };
    label.onEditorHide = [this] ()
        {
            label.setLookAndFeel(&lookAndFeel.inactive);
        };
    label.onEditorShow = [this] ()
        {
            if (auto* editor = label.getCurrentTextEditor())
            {
                editor->setInputRestrictions(0, "0123456789.");
                editor->setJustification(juce::Justification::centred);
                editor->setLookAndFeel(&lookAndFeel.active);
                label.setLookAndFeel(&lookAndFeel.active);
            }
        };

    addAndMakeVisible(label);
}

UI_Spinner::~UI_Spinner()
{
    attachedParameter = nullptr;
    label.setLookAndFeel(nullptr);
}

juce::String UI_Spinner::getDisplayString()
{
    return getTextFromValue(getValue());
}

juce::String UI_Spinner::getTextFromValue(double value)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(displayPrecision) << value;  // Set decimal precision to 2

    if (permanentDecimal && displayPrecision == 0)
    {
        ss << ".";
    }
    return ss.str();
}

void UI_Spinner::mouseDown(const juce::MouseEvent& e)
{
    label.setLookAndFeel(&lookAndFeel.active);

    if (isEnabled())
    {
        dragStartY = e.y;
        initialValue = getValue();
        lastDragTime = juce::Time::getCurrentTime();
        lastDragY = e.y;

        // Get the string that would be displayed (e.g., "123.45")
        auto displayString = getDisplayString();

        // Get the font and prepare the layout
        auto font = getLookAndFeel().getSliderPopupFont(*this);
        juce::GlyphArrangement ga;
        ga.addJustifiedText(font, displayString, 0.0f, 0.0f, (float) getWidth(), juce::Justification::centred);

        // Find the x-position of the decimal point
        float decimalX = -1.0f;
        for (int i = 0; i < ga.getNumGlyphs(); ++i)
        {
            auto g = ga.getGlyph(i);
            auto c = displayString[i];

            if (c == '.')
            {
                decimalX = g.getBounds().getX();
                break;
            }
        }

        // If decimal found, compare click position to it
        if (decimalX >= 0.0f)
        {
            isAdjustingDecimal = (e.x > decimalX) && displayPrecision > 0;
        }
        else
        {
            // No decimal — default to whole
            isAdjustingDecimal = false;
        }
    }
    juce::Slider::mouseDown(e);
}


void UI_Spinner::mouseDrag(const juce::MouseEvent& e)
{
    if (isEnabled())
    {
        const auto currentY = e.y;
        const auto currentTime = juce::Time::getCurrentTime();

        // Pixel movement since last drag
        int pixelDelta = std::abs(currentY - lastDragY);

        wasDragging = dragStartY >= 2;

        // Time since last drag in milliseconds
        int timeDeltaMs = (int) (currentTime.toMilliseconds() - lastDragTime.toMilliseconds());

        // Thresholds
        const int movementThreshold = 5;
        const int timeThresholdMs = 50;

        // Step sizes
        const float fineStep = 1.f;
        const float coarseStep = 5.f;

        float speed = (float) pixelDelta / (float) timeDeltaMs;
        float speedThreshold = 0.1f;

        float step = 0.0f;

        // DBG("Time: " << timeDeltaMs << "  Pixels: " << pixelDelta << "  Speed: " << speed);

        // Only apply step if enough pixels have been moved
        if (pixelDelta >= movementThreshold && speed > speedThreshold)
        {
            step = coarseStep;
            // DBG("Coarse: " << pixelDelta);
        }
        else if (pixelDelta >= movementThreshold)
        {
            step = fineStep;
            // DBG("Fine adjustment: " << pixelDelta);
        }
        else
        {
            // Not enough movement for even a fine adjustment
            // DBG("No adjustment: " << pixelDelta);
            return;
        }

        if (isAdjustingDecimal)
        {
            step *= 0.01f;
        }

        step /= movementThreshold;

        float newValue = getValue() - (currentY - lastDragY) * step;

        setValue(newValue, juce::sendNotificationAsync);

        lastDragTime = currentTime;
        lastDragY = currentY;
    }
}

void UI_Spinner::mouseUp(const juce::MouseEvent& e)
{
    juce::Slider::mouseUp(e);

    if (isEnabled())
    {
        if (wasDragging)
        {
            juce::NullCheckedInvocation::invoke(onUserEditComplete);
        }
        else
        {
            label.showEditor();
            if (auto* editor = label.getCurrentTextEditor())
            {
                editor->setJustification(juce::Justification::centred);
                editor->setLookAndFeel(&lookAndFeel.active);
            }
        }
    }

    label.setLookAndFeel(&lookAndFeel.inactive);

    wasDragging = false;
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
        valueChanged();
        setLocked(true);
    }
    else
    {
        setLocked(false);
    }

    Slider::setRange(min, max, interval);
}

void UI_Spinner::setAttachedParameter(juce::RangedAudioParameter* param)
{
    attachedParameter = param;
}

void UI_Spinner::setLocked(bool lock)
{
    if (lock)
    {
        wasEnabled = isEnabled() == true;
        setEnabled(false);
    }
    else
    {
        setEnabled(wasEnabled);
    }
}

void UI_Spinner::valueChanged()
{
    label.setText(getDisplayString(), juce::NotificationType::dontSendNotification);
    juce::NullCheckedInvocation::invoke(onValueChanged, getValue());
}

bool UI_Spinner::valueIsInRange(float value)
{
    auto range = attachedParameter->getNormalisableRange();
    return (value >= range.start && value <= range.end);
}