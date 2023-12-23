/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomKnob.h"

//==============================================================================
/**
*/
class PluginAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                    public juce::Button::Listener
{
public:
    PluginAudioProcessorEditor (PluginAudioProcessor&);
    ~PluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    virtual void buttonClicked(Button* button) override;
    void setBackground(juce::Colour colour);
    void loadButtonClicked();

    // For the FileChooser to load json models and IR files
    std::unique_ptr<FileChooser> myChooser;
    juce::String fname;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginAudioProcessor& audioProcessor;

    CustomKnob* knob1 = new CustomKnob("volume", audioProcessor);
    CustomKnob* knob2 = new CustomKnob("tone", audioProcessor);
    CustomKnob* knob3 = new CustomKnob("distortion", audioProcessor);

    TextButton loadButton;


    juce::Label knobLabel1, knobLabel2, knobLabel3;
    juce::Colour backgroundColour;

    ComboBox modelSelect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessorEditor)
};
