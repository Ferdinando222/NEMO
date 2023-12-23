/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginAudioProcessorEditor::PluginAudioProcessorEditor (PluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    addAndMakeVisible(knob1);
    addAndMakeVisible(knob2);
    addAndMakeVisible(knob3);

    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Import Tone");
    loadButton.setColour(juce::Label::textColourId, juce::Colours::black);
    loadButton.addListener(this);
}

PluginAudioProcessorEditor::~PluginAudioProcessorEditor()
{
}

//==============================================================================
void PluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)

    g.fillAll(backgroundColour);

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);

    // Add labels to the main component
    addAndMakeVisible(knobLabel1);
    addAndMakeVisible(knobLabel2);
    addAndMakeVisible(knobLabel3);

    // Set label texts
    knobLabel1.setText("Volume", juce::NotificationType::dontSendNotification);
    knobLabel2.setText("Tone", juce::NotificationType::dontSendNotification);
    knobLabel3.setText("Saturation", juce::NotificationType::dontSendNotification);

    knobLabel1.attachToComponent(knob1, false);
    knobLabel2.attachToComponent(knob2, false);
    knobLabel3.attachToComponent(knob3, false);

}

void PluginAudioProcessorEditor::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    float rowHeight = getHeight() / 5;


    // Position the knobs horizontally at the top
    knob1->setBounds(0, getHeight() / 3, getWidth() / 4, rowHeight * 1.5);
    knob2->setBounds(getWidth() / 4, getHeight() / 3, getWidth() / 4, rowHeight * 1.5);
    knob3->setBounds(getWidth() / 2, getHeight() / 3, getWidth() / 4, rowHeight * 1.5);

    //Position of the buttons

    loadButton.setBounds(getWidth() / 4, getHeight() - rowHeight * 1.5, getWidth() / 4, rowHeight * 1.5);

}


void PluginAudioProcessorEditor::setBackground(juce::Colour colour)
{
    backgroundColour = colour;
}


// funcrion for load Button 
void PluginAudioProcessorEditor::loadButtonClicked()
{
    myChooser = std::make_unique<FileChooser>("Select one or more .json tone files to import",
        File::getSpecialLocation(File::userDesktopDirectory),
        "*.json");

    auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::canSelectMultipleItems;

    myChooser->launchAsync(folderChooserFlags, [this](const FileChooser& chooser)
        {
            Array<File> files = chooser.getResults();
            for (auto file : files) {
                File fullpath = audioProcessor.userAppDataDirectory_tones.getFullPathName() + "/" + file.getFileName();
                bool b = fullpath.existsAsFile();
                if (b == false) {

                    audioProcessor.saveModel(file);
                    fname = file.getFileName();
                    audioProcessor.loaded_tone = file;
                    audioProcessor.loaded_tone_name = fname;
                    audioProcessor.custom_tone = 1;

                    // Copy selected file to model directory and load into dropdown menu
                    bool a = file.copyFileTo(fullpath);
                    if (a == true) {
                        modelSelect.addItem(file.getFileNameWithoutExtension(), audioProcessor.jsonFiles.size() + 1);
                        modelSelect.setSelectedItemIndex(audioProcessor.jsonFiles.size(), juce::NotificationType::dontSendNotification);
                        audioProcessor.jsonFiles.push_back(file);
                        audioProcessor.num_models += 1;
                    }
                    // Sort jsonFiles alphabetically
                    std::sort(audioProcessor.jsonFiles.begin(), audioProcessor.jsonFiles.end());
                }
            }
        });
    //setParamKnobColor();
}

void PluginAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &loadButton) {
        loadButtonClicked();
    }
    else
    {
        //loadIRClicked();
    }
}


