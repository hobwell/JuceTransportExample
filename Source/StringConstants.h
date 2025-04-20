/*
  ==============================================================================

    StringConstants.h
    Created: 5 Oct 2024 2:27:59pm
    Author:  Nicholas

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "KeyValuePair.h"

/// <summary>
/// ValueTree Parameter Identifiers
/// </summary>
namespace IDS
{
	// parameter ids
	const juce::String bar_length("bar_length");
	const juce::String beat_duration("beat_duration");
	const juce::String host_controls_play("host_controls_play");
	const juce::String host_controls_position("host_controls_position");
	const juce::String host_controls_tempo("host_controls_tempo");
	const juce::String host_controls_time_sig("host_controls_time_sig");
	const juce::String playing("playing");
	const juce::String ppq("ppq");
	const juce::String rewind_flag("rewind_flag");
	const juce::String sample_rate("sample_rate");
	const juce::String tempo("tempo");
	const juce::String tempo_duration("tempo_duration");
	const juce::String transport_tree("transport_tree");
}

/// <summary>
/// ValueTree Parameter Labels
/// </summary>
namespace LABELS
{
	// human-friendly names
	const juce::String bar_length("Bar Length");
	const juce::String beat_duration("Beat Duration");
	const juce::String host_controls_play_state("Host Controls Play");
	const juce::String host_controls_position_state("Host Controls Position");
	const juce::String host_controls_tempo_state("Host Controls Tempo");
	const juce::String host_controls_time_signature_state("Host Controls Time Signature");
	const juce::String playing("Playing");
	const juce::String ppq("PPQ");
	const juce::String rewind_flag("Rewind Flag");
	const juce::String sample_rate("Sample Rate");
	const juce::String tempo("Tempo");
	const juce::String tempo_duration("Tempo Duration");
}

namespace TEMPO
{
	const std::vector<std::pair<juce::String, float>> duration_options = {
		{"w", 1.f},
		{"h.", 0.75f},
		{"h", 0.5f},
		{"q.", 0.375f},
		{"q", 0.25f},
		{"e.", 0.1875f},
		{"e", 0.125f},
		{"s.", 0.09375f},
		{"s", 0.0625f},
		{"t.", 0.046875f},
		{"t", 0.03125f},
		{"i.", 0.0234375},
		{"i", 0.015625f},
		{"x.", 0.01171875f},
		{"x", 0.0078125f}
	};
}

/// <summary>
/// Unicode strings that represent icons in the GUI
/// </summary>
namespace UNICON
{
	const juce::String stop = juce::CharPointer_UTF8(u8"\x25A0");
	const juce::String play = juce::CharPointer_UTF8(u8"\x25B6");
	const juce::String rewind = juce::CharPointer_UTF8(u8"\x25C0\x25C0");
}