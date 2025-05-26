/*
  ==============================================================================

	TransportParameters.cpp
	Created: 3 Oct 2024 11:37:13pm
	Author:  Nicholas

  ==============================================================================
*/

#include "TransportParameters.h"

// struct TransportParameters

TransportParameters::TransportParameters(juce::AudioProcessor& processor, juce::UndoManager* undoManager, const juce::Identifier& valueTreeType) :
	parameters(createParameters()),
    undoManager(undoManager),
	_apvts(processor, undoManager, valueTreeType, createParameters()),
    apvts(_apvts)
{
	apvts.state.addListener(this);
	startTimerHz(30);
}

TransportParameters::~TransportParameters() {
	apvts.state.removeListener(this);
}

juce::AudioProcessorValueTreeState::ParameterLayout TransportParameters::createParameters()
{
	std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

	params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID {TRANSPORT::IDS::beat_duration, 1}, TRANSPORT::LABELS::beat_duration, 1, 16, 4));
	params.push_back (std::make_unique<juce::AudioParameterInt> (juce::ParameterID {TRANSPORT::IDS::bar_length, 1}, TRANSPORT::LABELS::bar_length, 1, 99, 4));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::host_controls_play, 1}, TRANSPORT::LABELS::host_controls_play_state, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::host_controls_position, 1}, TRANSPORT::LABELS::host_controls_position_state, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::host_controls_tempo, 1}, TRANSPORT::LABELS::host_controls_tempo_state, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::host_controls_tempo_speed, 1}, TRANSPORT::LABELS::host_controls_tempo_speed, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::host_controls_time_sig, 1}, TRANSPORT::LABELS::host_controls_time_signature_state, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::reposition_flag, 1}, TRANSPORT::LABELS::reposition_flag, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::playing, 1}, TRANSPORT::LABELS::playing, false));
	params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID {TRANSPORT::IDS::time_sig_controls_tempo_speed, 1}, TRANSPORT::LABELS::time_sig_controls_tempo_speed, false));
	params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {TRANSPORT::IDS::pos_bar, 1}, TRANSPORT::LABELS::pos_bar, 1.f, 99999.f, 1.f));
	params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {TRANSPORT::IDS::pos_beat, 1}, TRANSPORT::LABELS::pos_beat, 1.f, 99.f, 1.f));
	params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {TRANSPORT::IDS::pos_div, 1}, TRANSPORT::LABELS::pos_div, 1.f, 256.f, 1.f));
	params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {TRANSPORT::IDS::ppq, 1}, TRANSPORT::LABELS::ppq, -99999.f, 99999.f, 0)); // 27+hrs at 60bpm, 13+hrs @ 120bpm etc.
	params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID {TRANSPORT::IDS::sample_rate, 1}, TRANSPORT::LABELS::sample_rate, 0.f, 384000.f, 48000.f));
	
	// need to specify the step size for the tempo so that it can be exact
	juce::NormalisableRange<float> tempoSteppedRange(1.00, 999.0f, 0.01f);
	params.push_back (std::make_unique<juce::AudioParameterFloat> (
		juce::ParameterID {TRANSPORT::IDS::tempo, 1}, 
		TRANSPORT::LABELS::tempo, 
		tempoSteppedRange, 
		120.f
	));

    // need to specify the step size for the tempo duration so that it can be exact
    juce::NormalisableRange<float> tempoDurationSteppedRange(0.0078125f, 1.0f, 0.00390625f); // step size is 1/256
	params.push_back(std::make_unique<juce::AudioParameterFloat>(
		juce::ParameterID {TRANSPORT::IDS::tempo_speed, 1}, 
		TRANSPORT::LABELS::tempo_speed, 
		tempoDurationSteppedRange, 
		0.25f
	));
	//params.push_back(std::make_unique<SyncedAudioParameterFloat>(TRANSPORT::IDS::tempo_speed, TRANSPORT::LABELS::tempo_speed, tempoSteppedRange, 0.25f, apvts.state));

	return { params.begin(), params.end() };
}

std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> TransportParameters::createButtonAttachment(const juce::String& parameterID, juce::Button& button)
{
	return std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, parameterID, button);
}

std::unique_ptr<GenericComponentAttachment> TransportParameters::createGenericAttachment(const juce::String& parameterID, juce::Component& component, std::function<void(float)> paramToUi, std::function<void(std::function<void(float)>)> uiToParam)
{
	return std::make_unique<GenericComponentAttachment>(apvts, parameterID, component, paramToUi, uiToParam);
}

std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> TransportParameters::createSliderAttachment(const juce::String& parameterID, juce::Slider& slider)
{
	return std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, parameterID, slider);
}

void TransportParameters::flushPendingUpdates()
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

float TransportParameters::getPpq()
{
	return *apvts.getRawParameterValue(TRANSPORT::IDS::ppq);
}

void TransportParameters::setPpq(float ppq)
{
	apvts.getRawParameterValue(TRANSPORT::IDS::ppq)->store(ppq);
}

void TransportParameters::setPos(float ppq, bool forceUpdate)
{
	// Calculate the number of subdivisions per beat based on beat_duration
	int subDivisionsPerBeat = 16 / beat_duration;  // This adjusts based on beat duration

	// Calculate the scaler for converting PPQ to beats based on beat duration
	float quarterNotesPerBeat = 4.0f / beat_duration; // Adjust beat rate based on beat duration

	// convert ppq to number of total beats, based on the beat duration
	float beatPosition = ppq / quarterNotesPerBeat;

	updateParameter(TRANSPORT::IDS::ppq, ppq);
	updateParameter(TRANSPORT::IDS::pos_bar, 1 + ((int) beatPosition / bar_length), forceUpdate);
	updateParameter(TRANSPORT::IDS::pos_beat, 1 + ((int) beatPosition % bar_length), forceUpdate);
	updateParameter(TRANSPORT::IDS::pos_div, 1 + ((int) (beatPosition * subDivisionsPerBeat) % subDivisionsPerBeat), forceUpdate);
}

void TransportParameters::timerCallback()
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
void TransportParameters::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
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