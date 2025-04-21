/*
  ==============================================================================

	AVPTSWrapper.cpp
	Created: 14 Oct 2024 2:52:50pm
	Author:  Nicholas

  ==============================================================================
*/

#include "APVTSWrapper.h"
#include "SyncedAudioParameterFloat.h"

ApvtsWrapper::ApvtsWrapper(TransportTree* transport_tree, juce::UndoManager* undoManager) :
	tree(*transport_tree),
	apvts(transport_tree->apvts),
	undoManager(undoManager)
{
	apvts.state.addListener(this);
}

ApvtsWrapper::~ApvtsWrapper()
{
	apvts.state.removeListener(this);
}

float ApvtsWrapper::getPpq()
{
	return *apvts.getRawParameterValue(IDS::ppq);
}

void ApvtsWrapper::setPpq(float ppq)
{
	apvts.getRawParameterValue(IDS::ppq)->store(ppq);
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
	if (treeWhosePropertyHasChanged == apvts.state)
	{
        DBG("Property changed: " << property.toString() << " = " << apvts.state.getProperty(property).toString());
		if (property == juce::Identifier(IDS::tempo_relative_note_duration))
		{
			
		}
	}
}