/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RT_LSTM.h"


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
class PluginAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    PluginAudioProcessor();
    ~PluginAudioProcessor() override;

    //==============================================================================
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


    //==============================================================================
 
    
    void setValueKnob1(float knob_value);
    void setValueKnob2(float knob_value);
    void setValueKnob3(float knob_value);
    void saveModel(File configFile);
    void setUpDataDirectories();
    void addDirectory(const File& file);

    std::vector<File> jsonFiles;
    File currentDirectory = File::getCurrentWorkingDirectory().getFullPathName();
    File userAppDataDirectory = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile(JucePlugin_Manufacturer).getChildFile(JucePlugin_Name);
    File userAppDataDirectory_tones = userAppDataDirectory.getFullPathName() + "/tones";


    //Pedal/amp states
    int params = 0;
    int model_loaded = 0;
    float num_models = 0.0;
    int current_model_index = 0;
    const char* char_filename = "";
    bool lstm_state = true;
    int model_index = 0; // Used in processBlock when converting slider param to model index
    int amp_state = 1; // 0 = off, 1 = on
    int custom_tone = 0; // 0 = custom tone loaded, 1 = default channel tone
    File loaded_tone;
    juce::String loaded_tone_name;

    //model
    RT_LSTM LSTM;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginAudioProcessor)

    std::atomic<float>* inGainDbParam = nullptr;
    dsp::Gain<float> inputGain;
    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> dcBlocker;

    AudioParameterFloat* gainParam;
    AudioParameterFloat* masterParam;
    AudioParameterFloat* bassParam;
    AudioParameterFloat* midParam;
    AudioParameterFloat* trebleParam;
    AudioParameterFloat* presenceParam;
    AudioParameterFloat* modelParam;
    AudioParameterFloat* irParam;
    AudioParameterFloat* delayParam;
    AudioParameterFloat* reverbParam;

    bool fileSelected = 0;
    float knob_value1 = 0.0f;
    float knob_value2 = 0.0f;
    float knob_value3 = 0.0f;
    // models
    std::atomic<float>* modelTypeParam = nullptr;
 
    // example of model defined at compile-time

    const char* modelFilePath;
};
