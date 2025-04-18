/*
  ==============================================================================

	AVPTSWrapper.cpp
	Created: 14 Oct 2024 2:52:50pm
	Author:  Nicholas

  ==============================================================================
*/

#include "APVTSWrapper.h"

ApvtsWrapper::ApvtsWrapper(TransportTree* transportTree, juce::UndoManager* undoManager) :
	tree(*transportTree),
	apvts(transportTree->apvts),
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
