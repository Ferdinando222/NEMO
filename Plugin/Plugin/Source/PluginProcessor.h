/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "JuceHeader.h"
#include "RT_LSTM.h"
#include "AmpOSCReceiver.h"

#pragma once

#define GAIN_ID "gain"
#define GAIN_NAME "Gain"
#define MODEL_ID "model"
#define MODEL_NAME "Model"
#define IR_ID "ir"
#define IR_NAME "Ir"
#define MASTER_ID "master"
#define MASTER_NAME "Master"
#define BASS_ID "bass"
#define BASS_NAME "Bass"
#define MID_ID "mid"
#define MID_NAME "Mid"
#define TREBLE_ID "treble"
#define TREBLE_NAME "Treble"
#define PRESENCE_ID "presence"
#define PRESENCE_NAME "Presence"
#define DELAY_ID "delay"
#define DELAY_NAME "Delay"
#define REVERB_ID "reverb"
#define REVERB_NAME "Reverb"

//==============================================================================
/**
*/
class PluginAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    PluginAudioProcessor();
    ~PluginAudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    int getModelIndex(float model_param);
    //int getIrIndex(float ir_param);
    void loadConfig(juce::File configFile);
    //void loadIR(juce::File irFile);
    void setupDataDirectories();
    //void installTones();

    //void set_ampEQ(float bass_slider, float mid_slider, float treble_slider, float presence_slider);
    //void set_delayParams(float paramValue);
    //void set_reverbParams(float paramValue);

    float convertLogScale(float in_value, float x_min, float x_max, float y_min, float y_max);

    float decibelToLinear(float dbValue);

    void addDirectory(const juce::File& file);
    //void addDirectoryIR(const juce::File& file);
    void resetDirectory(const juce::File& file);
    //void resetDirectoryIR(const juce::File& file);
    std::vector<juce::File> jsonFiles;
    std::vector<juce::File> irFiles;
    juce::File currentDirectory = juce::File::getCurrentWorkingDirectory().getFullPathName();
    juce::File userAppDataDirectory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile(JucePlugin_Manufacturer).getChildFile(JucePlugin_Name);
    juce::File userAppDataDirectory_tones = userAppDataDirectory.getFullPathName() + "/tones";
    juce::File userAppDataDirectory_irs = userAppDataDirectory.getFullPathName() + "/irs";

    // Pedal/amp states
    int amp_state = 1; // 0 = off, 1 = on
    int custom_tone = 0; // 0 = custom tone loaded, 1 = default channel tone
    juce::File loaded_tone;
    juce::String loaded_tone_name;
    const char* char_filename = "";
    int model_loaded = 0;
    int current_model_index = 0;
    float num_models = 0.0;
    int model_index = 0; // Used in processBlock when converting slider param to model index
    bool lstm_state = true;

    juce::String loaded_ir_name;
    float num_irs = 0.0;
    int ir_loaded = 0;
    int custom_ir = 0; // 0 = custom tone loaded, 1 = default channel tone
    juce::File loaded_ir;
    bool ir_state = true;
    int current_ir_index = 0;
    int ir_index = 0;

    // The number of parameters for the model
    // 0 is for a snap shot model
    // The PluginEditor uses this to determin which knobs to color red
    int params = 0;

    RT_LSTM LSTM;

    dsp::Reverb::Parameters rev_params;


private:
    var dummyVar;

    juce::AudioParameterFloat* gainParam;
    juce::AudioParameterFloat* masterParam;
    juce::AudioParameterFloat* bassParam;
    juce::AudioParameterFloat* midParam;
    juce::AudioParameterFloat* trebleParam;
    juce::AudioParameterFloat* presenceParam;
    juce::AudioParameterFloat* modelParam;
    juce::AudioParameterFloat* irParam;
    juce::AudioParameterFloat* delayParam;
    juce::AudioParameterFloat* reverbParam;

    dsp::IIR::Filter<float> dcBlocker;


    // Effects
    enum
    {
        delayIndex,
        reverbIndex
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAudioProcessor)
};