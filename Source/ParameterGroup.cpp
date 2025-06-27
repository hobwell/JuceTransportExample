/*
  ==============================================================================

    ParameterGroup.cpp
    Created: 31 May 2025 10:44:31am
    Author:  Nicholas

  ==============================================================================
*/

#include "ParameterGroup.h"

// struct TransportParameters

ParameterGroup::ParameterGroup(juce::UndoManager* undoManager, juce::AudioProcessorValueTreeState& apvts) :
    undoManager(undoManager),
    apvts(apvts)
{
    apvts.state.addListener(this);
    startTimerHz(30);
}

ParameterGroup::~ParameterGroup()
{
    apvts.state.removeListener(this);
}

std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ParameterGroup::createButtonAttachment(const juce::String& parameterID, juce::Button& button)
{
    return std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, parameterID, button);
}

std::unique_ptr<GenericComponentAttachment> ParameterGroup::createGenericAttachment(const juce::String& parameterID, juce::Component& component, std::function<void(float)> paramToUi, std::function<void(std::function<void(float)>)> uiToParam)
{
    return std::make_unique<GenericComponentAttachment>(apvts, parameterID, component, paramToUi, uiToParam);
}

std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ParameterGroup::createSliderAttachment(const juce::String& parameterID, juce::Slider& slider)
{
    return std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, parameterID, slider);
}

void ParameterGroup::flushPendingUpdates()
{
    std::unordered_map<juce::String, std::function<void()>> updatesToApply;

    {
        juce::SpinLock::ScopedLockType lock(pendingUpdatesLock);
        updatesToApply = std::move(pendingUpdates);
        pendingUpdates.clear();
    }

    for (auto& [paramId, updater] : updatesToApply)
    {
        // safely call the updater on the message thread
        updateOnMessageThread([updater]
            {
                updater(); // safely call them OUTSIDE the lock
            });
    }
}

void ParameterGroup::timerCallback()
{
    // flush any pending updates
    flushPendingUpdates();
}

/// <summary>
/// Respond to changes in the APVTS
/// </summary>
/// <remarks>
/// When a parameter in the tree changes, this method will trigger on every 
/// instance of the wrapper - consider passing a single instance of the 
/// wrapper around, rather than creating multiple instances.
/// </remarks>
void ParameterGroup::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    // When parameters are changed by attachments, the value tree is not automatically update, so we have to manually sync the values.
    if (treeWhosePropertyHasChanged.hasType("PARAM") && property == juce::Identifier("value"))
    {
        const auto changePropertyId = treeWhosePropertyHasChanged.getProperty("id");

        if (!changePropertyId.isString())
            return;

        const auto paramId = juce::Identifier(changePropertyId.toString());
        const auto newValue = treeWhosePropertyHasChanged.getProperty("value");

        if (apvts.state.getProperty(paramId) != newValue)
        {
            apvts.state.setProperty(paramId, newValue, nullptr);
            // DBG("Synced param '" << paramId.toString() << "' to transport_tree: " << newValue.toString());
        }
    }

    if (treeWhosePropertyHasChanged == apvts.state)
    {
        // DBG("Property changed: " << property.toString() << " = " << apvts.state.getProperty(property).toString());
    }
}