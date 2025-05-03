/*
  ==============================================================================

    FontAwesome_LookAndFeel.cpp
    Created: 30 Jun 2024 12:14:14pm
    Author:  Nicholas

  ==============================================================================
*/

#include "FontAwesome_LookAndFeel.h"

FontAwesome_LookAndFeel::FontAwesome_LookAndFeel() {
    fontAwesome = fonts->getFontAwesome();
}

FontAwesome_LookAndFeel::~FontAwesome_LookAndFeel() {}

juce::Font FontAwesome_LookAndFeel::getLabelFont(juce::Label& label) {
    int paddedHeight = label.getHeight() - (2*verticalPadding);
    fontAwesome.setHeight(paddedHeight);
    return fontAwesome;
}

juce::Font FontAwesome_LookAndFeel::getTextButtonFont(juce::TextButton& button, int buttonHeight) {
    int paddedHeight = buttonHeight - (2 * verticalPadding);
    fontAwesome.setHeight(paddedHeight);
    return fontAwesome;
}