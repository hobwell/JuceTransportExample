/*
  ==============================================================================

	FontAwesome_LookAndFeel.h
	Created: 30 Jun 2024 12:14:14pm
	Author:  Nicholas

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Fonts.h"

class FontAwesome_LookAndFeel : public juce::LookAndFeel_V4 {
public:

#pragma region icons

	const juce::CharPointer_UTF8 icon_arrow_right_long = juce::CharPointer_UTF8(u8"\xf178"); // duration?
	const juce::CharPointer_UTF8 icon_arrow_right_from_bracket = juce::CharPointer_UTF8(u8"\xf08b"); // delay?
	const juce::CharPointer_UTF8 icon_arrows_up_down_left_right = juce::CharPointer_UTF8(u8"\xf047"); // move
	const juce::CharPointer_UTF8 icon_backward = juce::CharPointer_UTF8(u8"\xf04a"); // rewind
	const juce::CharPointer_UTF8 icon_circle = juce::CharPointer_UTF8(u8"\xf111"); // record
	const juce::CharPointer_UTF8 icon_clock = juce::CharPointer_UTF8(u8"\xf017");
	const juce::CharPointer_UTF8 icon_gear = juce::CharPointer_UTF8(u8"\xf013"); // settings
	const juce::CharPointer_UTF8 icon_hourglass_start = juce::CharPointer_UTF8(u8"\xf251");
	const juce::CharPointer_UTF8 icon_music = juce::CharPointer_UTF8(u8"\xf001"); // music / notes
	const juce::CharPointer_UTF8 icon_play = juce::CharPointer_UTF8(u8"\xf04b"); // play
	const juce::CharPointer_UTF8 icon_plus = juce::CharPointer_UTF8(u8"\x2b"); // add
	const juce::CharPointer_UTF8 icon_repeat = juce::CharPointer_UTF8(u8"\xf363"); // period?
	const juce::CharPointer_UTF8 icon_stop = juce::CharPointer_UTF8(u8"\xf04d"); // stop
	const juce::CharPointer_UTF8 icon_volume_high = juce::CharPointer_UTF8(u8"\xf028"); // enable
	const juce::CharPointer_UTF8 icon_volume_xmark = juce::CharPointer_UTF8(u8"\xf6a9");// disable

	const juce::CharPointer_UTF8 ctl_add_interval = juce::CharPointer_UTF8(u8"\x2b\xf001");// plus + music
	

#pragma endregion

	FontAwesome_LookAndFeel();
	~FontAwesome_LookAndFeel();

	juce::Font getLabelFont(juce::Label& label) override;
	juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;

private:
	int horizontalPadding = 5;
	int verticalPadding = 4;

	juce::SharedResourcePointer<Fonts> fonts;
	juce::Font fontAwesome;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FontAwesome_LookAndFeel)
};