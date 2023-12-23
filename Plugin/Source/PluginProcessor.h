#pragma once
#include <JuceHeader.h>
#include "RT_LSTM.h"

namespace fs = std::filesystem;


class NeuralPluginAudioProcessor : public juce::AudioProcessor

{
public:
    //==============================================================================
    NeuralPluginAudioProcessor();
    ~NeuralPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    //==============================================================================
    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    void setValueKnob1(float knob_value);
    void setValueKnob2(float knob_value);
    void setValueKnob3(float knob_value);
    void setFilePath(const char* path);
    void saveModel();
    void loadConfig(File configFile);

    RT_LSTM LSTM;
    std::vector<File> jsonFiles;


    // Pedal/amp states
    int amp_state = 1; // 0 = off, 1 = on
    int custom_tone = 0; // 0 = custom tone loaded, 1 = default channel tone
    File loaded_tone;
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
    File loaded_ir;
    bool ir_state = true;
    int current_ir_index = 0;
    int ir_index = 0;

    // The number of parameters for the model
    // 0 is for a snap shot model
    // The PluginEditor uses this to determin which knobs to color red
    int params = 0;


private:
    //==============================================================================
    juce::AudioProcessorValueTreeState parameters;
    std::atomic<float>* inGainDbParam = nullptr;
    dsp::Gain<float> inputGain;
    dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> dcBlocker;

    bool fileSelected = 0;
    float knob_value1 = 0.0f;
    float knob_value2 = 0.0f;
    float knob_value3 = 0.0f;
    // models
    std::atomic<float>* modelTypeParam = nullptr;
    std::unique_ptr<RTNeural::Model<float>> models[2];
    // example of model defined at compile-time

    const char* modelFilePath;
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NeuralPluginAudioProcessor)
};