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
}

APVTSWrapper::~APVTSWrapper()
{
}