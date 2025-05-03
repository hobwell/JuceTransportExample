/*
  ==============================================================================

    AVPTSWrapper.cpp
    Created: 14 Oct 2024 2:52:50pm
    Author:  Nicholas

  ==============================================================================
*/

#include "APVTSWrapper.h"

ApvtsWrapper::ApvtsWrapper(TransportTree* transport_tree, juce::UndoManager* undoManager) :
    tree(*transport_tree),
    apvts(transport_tree->apvts),
    undoManager(undoManager)
{
    apvts.state.addListener(this);
    startTimerHz(30);
}

ApvtsWrapper::~ApvtsWrapper()
{
    apvts.state.removeListener(this);
}

void ApvtsWrapper::flushPendingUpdates()
{
    std::unordered_map<juce::String, std::function<void()>> updatesToApply;

    {
        juce::SpinLock::ScopedLockType lock(pendingUpdatesLock);
        updatesToApply = std::move(pendingUpdates);
        pendingUpdates.clear();
    }

    for (auto& [paramId, updater] : updatesToApply)
        updater(); // safely call them OUTSIDE the lock
}

float ApvtsWrapper::getPpq()
{
    return *apvts.getRawParameterValue(IDS::ppq);
}

void ApvtsWrapper::setPpq(float ppq)
{
    apvts.getRawParameterValue(IDS::ppq)->store(ppq);
}

void ApvtsWrapper::setPos(float ppq)
{
    // Calculate the number of subdivisions per beat based on beat_duration
    int subDivisionsPerBeat = 16 / beat_duration;  // This adjusts based on beat duration

    // Calculate the scaler for converting PPQ to beats based on beat duration
    float quarterNotesPerBeat = 4.0f / beat_duration; // Adjust beat rate based on beat duration

    // convert ppq to number of total beats, based on the beat duration
    float beatPosition = ppq / quarterNotesPerBeat;

    updateParameter(IDS::ppq, ppq);
    updateParameter(IDS::pos_bar, 1 + ((int) beatPosition / bar_length));
    updateParameter(IDS::pos_beat, 1 + ((int) beatPosition % bar_length));
    updateParameter(IDS::pos_div, 1 + ((int) (beatPosition * subDivisionsPerBeat) % subDivisionsPerBeat));
}

void ApvtsWrapper::timerCallback()
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
void ApvtsWrapper::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
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

