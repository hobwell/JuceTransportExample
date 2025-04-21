/*
  ==============================================================================

    CustomAudioPlayHead.h
    Created: 16 Sep 2024 5:32:45pm
    Author:  Nicholas

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TransportTree.h"
#include "APVTSWrapper.h"

/// <summary>
/// A custom playhed that will sync changes to transport information into an 
/// APVTS, whether the plugin is running in standalone mode or within a host.
/// 
/// When running in a host, this is primarily needed to sync changes from 
/// the host to the APVTS.
/// 
/// When running in standalone mode, this is primarily needed used to pull the 
/// current transport values from the APVTS that were updated by the UI.
/// </summary>
class CustomPlayHead : public juce::AudioPlayHead
{
public:
    CustomPlayHead(juce::AudioProcessor& proc, double sampleRateIn, ApvtsWrapper& wrapper);

    ~CustomPlayHead() override;

    juce::Optional<PositionInfo> getPosition(int bufferSize) const;

    void synchronizeState();
private:
    ApvtsWrapper& transportWrapper;

    juce::AudioProcessor& processor;
    juce::AudioPlayHead::PositionInfo& info = juce::AudioPlayHead::PositionInfo();
    juce::AudioPlayHead::PositionInfo& hostInfo = juce::AudioPlayHead::PositionInfo();

    // internal transport info - represents the current state of the playhead
    bool isPlaying = false;
    mutable double ppq = 0.0;
    mutable uint64_t bufferEnd = 0;
    mutable uint64_t bufferStart = 0;
    double sampleRate = 48000.0;
    mutable double tempo = 120.0;
    float tempoRelativeNoteDuration = 0.25f; // what fraction to use for the tempo relative note duration (0.25 = quarter note, 0.5 = half note, etc.)
    mutable uint64_t timeNs = 0;
    juce::AudioPlayHead::TimeSignature timeSig {4, 4};

    // external transport info - represents the "external" state of the transport (from either the host or the GUI)
    bool isStandalone = false;
    float nextTempo;
    float nextTempoRelativeNoteDuration;
    bool nextPlaying;
    juce::AudioPlayHead::TimeSignature nextTimeSig {4, 4};

    bool useThisPlayhead = false;

    // for calculating ppq (in the absence of host position info)
    mutable bool needsUpdate = false;
    mutable int bufferSize = 480;
    mutable float samplesPerBeat = sampleRate * secondsPerBeat;
    mutable float secondsPerBeat = 60.f / tempo; // 120 tempo
    mutable float beatsPerQuarterNote = (1.f / timeSig.denominator) / tempoRelativeNoteDuration;
    mutable float quarterNotesPerBuffer = bufferSize / (samplesPerBeat * beatsPerQuarterNote);

    juce::Optional<PositionInfo> getPosition() const override;

    template <typename TypeName>
    void updateParameter(const juce::String& paramId, TypeName value)
    {
        if (auto* param = transportWrapper.tree.apvts.getParameter(paramId))
        {
            // for some reason, this is needed to ensure controls are populated with the correct value before they are visible in the host
            transportWrapper.apvts.state.setProperty(juce::Identifier(paramId), value, nullptr);

            // normalise the value to the range of the parameter
            auto range = param->getNormalisableRange();

            if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
            {
                float normalized = range.convertTo0to1(static_cast<float>(value));
                floatParam->setValueNotifyingHost(normalized);
            }
            else if (auto* intParam = dynamic_cast<juce::AudioParameterInt*>(param))
            {
                // Handle int type parameters
                float normalized = range.convertTo0to1(static_cast<int>(value));
                intParam->setValueNotifyingHost(normalized);
            }
            else if (auto* boolParam = dynamic_cast<juce::AudioParameterBool*>(param))
            {
                // Handle bool type parameters
                boolParam->setValueNotifyingHost(static_cast<bool>(value));
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

    void updatePosition() const;

    void recalculate() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomPlayHead);
};