/*
  ==============================================================================

    TransportParameters.h
    Created: 3 Oct 2024 11:37:13pm
    Author:  Nicholas

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "StringConstants.h"
#include "GenericComponentAttachment.h"

/// <summary>
/// Contains and initializes an AudioProcessorValueTreeState for the transport 
/// parameters of the plugin.
/// </summary>
struct TransportParameters :
    juce::ValueTree::Listener,
    juce::Timer
{
    juce::AudioProcessorValueTreeState _apvts;
    juce::AudioProcessorValueTreeState& apvts;

    juce::AudioProcessorValueTreeState::ParameterLayout parameters;
    juce::UndoManager* undoManager;

    juce::CachedValue<int> bar_length {apvts.state, IDS::bar_length, undoManager, 4};
    juce::CachedValue<int> beat_duration {apvts.state, IDS::beat_duration, undoManager, 4};
    juce::CachedValue<bool> host_controls_playing {apvts.state, IDS::host_controls_play, undoManager, false};
    juce::CachedValue<bool> host_controls_position {apvts.state, IDS::host_controls_position, undoManager, false};
    juce::CachedValue<bool> host_controls_tempo {apvts.state, IDS::host_controls_tempo, undoManager, false};
    juce::CachedValue<bool> host_controls_tempo_speed {apvts.state, IDS::host_controls_tempo_speed, undoManager, false};
    juce::CachedValue<bool> host_controls_time_signature {apvts.state, IDS::host_controls_time_sig, undoManager, false};
    juce::CachedValue<bool> playing {apvts.state, IDS::playing, undoManager, false};
    juce::CachedValue<bool> rewind_flag {apvts.state, IDS::rewind_flag, undoManager, false};
    juce::CachedValue<bool> time_sig_controls_tempo_speed {apvts.state, IDS::time_sig_controls_tempo_speed, undoManager, false};
    // ppq is not a good candidate for a cached value, as it is frequently updated
    // juce::CachedValue<float> ppq{ apvts.state, IDS::ppq, undoManager, 0.f };
    // pos_* are not good candidates for cached values, as they are frequently updated
    // juce::CachedValue<float> pos_bar{ apvts.state, IDS::pos_bar, undoManager, 0.f };
    // juce::CachedValue<float> pos_beat{ apvts.state, IDS::pos_beat, undoManager, 0.f };
    // juce::CachedValue<float> pos_div{ apvts.state, IDS::pos_div, undoManager, 0.f };
    juce::CachedValue<float> sample_rate {apvts.state, IDS::sample_rate, undoManager, 384000.f};
    juce::CachedValue<float> tempo {apvts.state, IDS::tempo, undoManager, 120.f};
    juce::CachedValue<float> tempo_speed {apvts.state, IDS::tempo_speed, undoManager, 0.25f};

    TransportParameters(juce::AudioProcessor& processor, juce::UndoManager* undoManager, const juce::Identifier& valueTreeType);
    ~TransportParameters();

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> createButtonAttachment(const juce::String& parameterID, juce::Button& button);
    std::unique_ptr<GenericComponentAttachment> createGenericAttachment(const juce::String& parameterID, juce::Component& component, std::function<void(float)> paramToUi, std::function<void(std::function<void(float)>)> uiToParam);
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> createSliderAttachment(const juce::String& parameterID, juce::Slider& slider);

    void flushPendingUpdates();
    float getPpq();
    void setPpq(float ppq);
    void setPos(float ppq);
    void timerCallback() override;
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;

    template <typename TypeName>
    void updateParameter(const juce::String& paramId, TypeName value) const
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

                if (*floatParam == static_cast<float>(value))
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
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportParameters)
};