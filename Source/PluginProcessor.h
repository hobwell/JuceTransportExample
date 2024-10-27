/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomPlayHead.h"
#include "TransportTree.h"

//==============================================================================
/**
*/
class TransportExampleAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TransportExampleAudioProcessor();
    ~TransportExampleAudioProcessor() override;

    //==============================================================================
    // Q: Why did I make these shared pointers?
    // A: Every instance of the plugin will share the same transport within a host, 
    // so it makes sense to use a single transport tree for all instances.  I 
    // decided that this limitation should also be applied when running in 
    // standalone mode.
    std::shared_ptr<TransportTree> transportTree; 
    std::shared_ptr<APVTSWrapper> transportWrapper;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    // a custom playhead that will unify transport data whether the plugin is running in standalone or in a host
    CustomPlayHead* customPlayHead{ nullptr };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportExampleAudioProcessor)
};
