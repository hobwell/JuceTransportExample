/*
  ==============================================================================

	AVPTSWrapper.cpp
	Created: 14 Oct 2024 2:52:50pm
	Author:  Nicholas

  ==============================================================================
*/

#include "APVTSWrapper.h"

APVTSWrapper::APVTSWrapper(TransportTree* transportTree, juce::UndoManager* undoManager) :
	tree(*transportTree),
	apvts(transportTree->apvts),
	undoManager(undoManager)
{
	apvts.state.addListener(this);
}

APVTSWrapper::~APVTSWrapper()
{
	apvts.state.removeListener(this);
}

/// <summary>
/// Respond to changes in the APVTS
/// </summary>
/// <remarks>
/// When a parameter in the tree changes, this method will trigger on every 
/// instance of the wrapper - consider passing a single instance of the 
/// wrapper around, rather than creating multiple instances.
/// </remarks>
void APVTSWrapper::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
	if (treeWhosePropertyHasChanged == apvts.state) {
		if (property == bar_length.getPropertyID()) {
			bar_length.forceUpdateOfCachedValue();
			//apvts.getRawParameterValue(IDS::bar_length)->store(*bar_length);
		} else if (property == beat_duration.getPropertyID()) {
			beat_duration.forceUpdateOfCachedValue();
			apvts.getRawParameterValue(IDS::beat_duration)->store(*beat_duration);
		} else if (property == playing.getPropertyID()) {
			playing.forceUpdateOfCachedValue();
			apvts.getRawParameterValue(IDS::playing)->store(*playing);
		} else if (property == rewind_flag.getPropertyID()) {
			rewind_flag.forceUpdateOfCachedValue();
			apvts.getRawParameterValue(IDS::rewind_flag)->store(*rewind_flag);
		} else if (property == ppq.getPropertyID()) {
			ppq.forceUpdateOfCachedValue();
			apvts.getRawParameterValue(IDS::ppq)->store(*ppq);
		} else if (property == sample_rate.getPropertyID()) {
			sample_rate.forceUpdateOfCachedValue();
			apvts.getRawParameterValue(IDS::sample_rate)->store(*sample_rate);
		} else if (property == tempo.getPropertyID()) {
			tempo.forceUpdateOfCachedValue();
			// DBG("A Wrapper: " << *tempo << "  APVTS: " << *apvts.getRawParameterValue(IDS::tempo));

			// setting tempo from the UI thread is causing an error here when this listener is triggered (vector erase iterator outside range?)
			// I don't understand why this is necessary, I thought the cached value would update the avpts parameter automatically
			apvts.getRawParameterValue(IDS::tempo)->store(*tempo); // <-- doesn't trigger the slider attachment - but does update the APVTS parameter

			// do whatever is needed when the tempo changes...
			juce::NullCheckedInvocation::invoke(onTempoChanged, tempo); // use an event handler to update the slider (if the host is in control: see UI_Transport.tempoSetup())
			
			// verify that the tempo updated
			// DBG("B Wrapper: " << *tempo << "  APVTS: " << *apvts.getRawParameterValue(IDS::tempo));
			// jassert(*tempo == *apvts.getRawParameterValue(IDS::tempo));
		}
	}
}