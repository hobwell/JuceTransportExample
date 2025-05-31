/*
  ==============================================================================

    ParameterGroup.h
    Created: 31 May 2025 10:44:31am
    Author:  Nicholas

  ==============================================================================
*/

#pragma once
#pragma once
#include <JuceHeader.h>
#include "GenericComponentAttachment.h"

/// <summary>
/// Contains and initializes an AudioProcessorValueTreeState for the transport 
/// parameters of the plugin.
/// </summary>
struct ParameterGroup :
    juce::ValueTree::Listener,
    juce::Timer
{

    ParameterGroup(juce::UndoManager* undoManager, juce::AudioProcessorValueTreeState& apvts);
    virtual ~ParameterGroup();

    juce::AudioProcessorValueTreeState& apvts;

    juce::AudioProcessorValueTreeState::ParameterLayout parameters;
    juce::UndoManager* undoManager;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> createButtonAttachment(const juce::String& parameterID, juce::Button& button);
    std::unique_ptr<GenericComponentAttachment> createGenericAttachment(const juce::String& parameterID, juce::Component& component, std::function<void(float)> paramToUi, std::function<void(std::function<void(float)>)> uiToParam);
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> createSliderAttachment(const juce::String& parameterID, juce::Slider& slider);

    void flushPendingUpdates();
    void timerCallback() override;
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

    template <typename TypeName>
    void updateParameter(const juce::String& paramId, TypeName value, bool forceUpdate = false) const
    {
        if (auto* param = apvts.getParameter(paramId))
        {
            // for some reason, this is needed to ensure controls are populated with the correct value before they are visible in the host
            // apvts.state.setProperty(juce::Identifier(paramId), value, nullptr);

            // normalise the value to the range of the parameter
            auto range = param->getNormalisableRange();

            if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
            {
                // handle float parameters
                auto range = floatParam->getNormalisableRange();
                auto normalized = range.convertTo0to1(static_cast<float>(value));

                if (*floatParam == static_cast<float>(value) && ! forceUpdate)
                    return; // Already the correct value, no need to push update!

                const auto valueCopy = normalized;
                const auto floatParamCopy = floatParam;
                const auto paramIdCopy = paramId;

                juce::SpinLock::ScopedLockType lock(pendingUpdatesLock);
                pendingUpdates[paramId] = [this, paramIdCopy, floatParamCopy, valueCopy] ()
                    {
                        floatParamCopy->setValueNotifyingHost(valueCopy);
                        apvts.state.setProperty(paramIdCopy, floatParamCopy->get(), nullptr); // <- set ValueTree property too - only really needed for debugging via propertyChanged
                    };
            }
            else if (auto* intParam = dynamic_cast<juce::AudioParameterInt*>(param))
            {
                // handle int parameters
                auto range = intParam->getNormalisableRange();
                auto normalized = range.convertTo0to1(static_cast<int>(value));

                if (*intParam == static_cast<int>(value))
                    return; // Already the correct value, no need to push update!

                const auto valueCopy = normalized;
                const auto intParamCopy = intParam;
                const auto paramIdCopy = paramId;

                juce::SpinLock::ScopedLockType lock(pendingUpdatesLock);
                pendingUpdates[paramId] = [this, paramIdCopy, intParamCopy, valueCopy] ()
                    {
                        intParamCopy->setValueNotifyingHost(valueCopy);
                        apvts.state.setProperty(paramIdCopy, intParamCopy->get(), nullptr); // <- set ValueTree property too - only really needed for debugging via propertyChanged
                    };
            }
            else if (auto* boolParam = dynamic_cast<juce::AudioParameterBool*>(param))
            {
                // handle bool parameters
                const auto boolValue = static_cast<bool>(value) ? 1.0f : 0.0f;

                if (*boolParam == boolValue)
                    return; // Already the correct value, no need to push update!

                const auto boolParamCopy = boolParam;
                const auto paramIdCopy = paramId;

                juce::SpinLock::ScopedLockType lock(pendingUpdatesLock);
                pendingUpdates[paramId] = [this, paramIdCopy, boolParamCopy, boolValue] ()
                    {
                        boolParamCopy->setValueNotifyingHost(boolValue);
                        apvts.state.setProperty(paramIdCopy, boolParamCopy->get(), nullptr); // <- set ValueTree property too - only really needed for debugging via propertyChanged
                    };
            }
            else
            {
                // Optionally log if the parameter type doesn't match
                DBG("Unsupported parameter type: " << paramId);
            }
        }
        else
        {
            DBG("Parameter with ID " << paramId << " not found in APVTS.");
        }
    }

private:

    mutable juce::SpinLock pendingUpdatesLock;

    // When updateParameter() is called, it just overwrites the last update for that parameterId.
    // If multiple calls happen, only the last value stays in the queue.
    mutable std::unordered_map<juce::String, std::function<void()>> pendingUpdates;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterGroup)
};