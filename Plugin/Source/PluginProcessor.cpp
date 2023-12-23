#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <juce_core/juce_core.h> 
//==============================================================================

NeuralPluginAudioProcessor::NeuralPluginAudioProcessor() :
#if JUCE_IOS || JUCE_MAC
    AudioProcessor(juce::JUCEApplicationBase::isStandaloneApp() ?
        BusesProperties().withInput("Input", juce::AudioChannelSet::mono(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true) :
        BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
#else
    AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
#endif
    parameters(*this, nullptr, Identifier("Parameters"),
        {
            std::make_unique<AudioParameterFloat>("gain_db", "Gain [dB]", -12.0f, 12.0f, 0.0f),
            std::make_unique<AudioParameterChoice>("model_type", "Model Type", StringArray { "Run-Time", "Compile-Time" }, 0)
        })
{
    inGainDbParam = parameters.getRawParameterValue("gain_db");
    modelTypeParam = parameters.getRawParameterValue("model_type");

    std::sort(jsonFiles.begin(), jsonFiles.end());
    if (jsonFiles.size() > 0) {
        loadConfig(jsonFiles[current_model_index]);
    }

    DBG("LEGGO");

}

NeuralPluginAudioProcessor::~NeuralPluginAudioProcessor()
{
}

//==============================================================================
const String NeuralPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NeuralPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool NeuralPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool NeuralPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double NeuralPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NeuralPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int NeuralPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NeuralPluginAudioProcessor::setCurrentProgram(int index)
{
}

const String NeuralPluginAudioProcessor::getProgramName(int index)
{
    return {};
}

void NeuralPluginAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void NeuralPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{

    LSTM.reset();

    *dcBlocker.state = *dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 35.0f);

    dsp::ProcessSpec spec{ sampleRate, static_cast<uint32> (samplesPerBlock), 2 };
    inputGain.prepare(spec);
    inputGain.setRampDurationSeconds(0.05);
    dcBlocker.prepare(spec);
}

void NeuralPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool NeuralPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void NeuralPluginAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    if (fileSelected) {
        dsp::AudioBlock<float> block(buffer);
        dsp::ProcessContextReplacing<float> context(block);

        juce::ScopedNoDenormals noDenormals;
        auto totalNumInputChannels = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();

        // Setup Audio Data
        const int numSamples = buffer.getNumSamples();
        const int numInputChannels = getTotalNumInputChannels();
        const int sampleRate = getSampleRate();

        const int gain = 1;
        const int master = 1;


        // Apply LSTM model
        if (model_loaded == 1 && lstm_state == true) {
            if (current_model_index != model_index) {
                loadConfig(jsonFiles[model_index]);
                current_model_index = model_index;
            }

            // Process LSTM based on input_size (snapshot model or conditioned model)
            if (LSTM.input_size == 1) {
                LSTM.process(buffer.getReadPointer(0), buffer.getWritePointer(0), numSamples);
            }
            else if (LSTM.input_size == 2) {
                LSTM.process(buffer.getReadPointer(0), gain, buffer.getWritePointer(0), numSamples);
            }
            else if (LSTM.input_size == 3) {
                LSTM.process(buffer.getReadPointer(0), gain, master, buffer.getWritePointer(0), numSamples);
            }
        }

        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear(i, 0, buffer.getNumSamples());


        ignoreUnused(midiMessages);
    }
}

//==============================================================================
bool NeuralPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NeuralPluginAudioProcessor::createEditor()
{
    return new NeuralPluginAudioProcessorEditor(*this);
}

//==============================================================================
void NeuralPluginAudioProcessor::getStateInformation(MemoryBlock& destData)
{

}

void NeuralPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{

}

void NeuralPluginAudioProcessor::setValueKnob1(float knob_value)
{
    knob_value1 = knob_value;
}

void NeuralPluginAudioProcessor::setValueKnob2(float knob_value)
{
    knob_value2 = knob_value;
}

void NeuralPluginAudioProcessor::setValueKnob3(float knob_value)
{
    knob_value3 = knob_value;
}

void NeuralPluginAudioProcessor::setFilePath(const char* path)
{
    modelFilePath = path;
    std::ifstream jsonStream(path, std::ifstream::binary);
    nlohmann::json jsonInput;
    jsonStream >> jsonInput;
    models[0] = RTNeural::json_parser::parseJson<float>(jsonInput);
    std::ifstream jsonStream1(path, std::ifstream::binary);
    nlohmann::json jsonInput1;
    jsonStream1 >> jsonInput1;
    models[1] = RTNeural::json_parser::parseJson<float>(jsonInput1);
    models[0]->reset();
    models[1]->reset();
    if(!fileSelected) {
        fileSelected = true;
    }
}

void NeuralPluginAudioProcessor::saveModel()
{
    if (modelFilePath != nullptr)
    {
        DBG(modelFilePath);

        juce::File pluginFile = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile);
        juce::File dataFolder = pluginFile.getParentDirectory().getChildFile("model");

        if (!dataFolder.exists())
        {
            dataFolder.createDirectory();
        }
        DBG(dataFolder.getFullPathName());

        juce::File originalFile(modelFilePath);
        juce::File copiedFile = dataFolder.getChildFile(originalFile.getFileName());

        if (originalFile.copyFileTo(copiedFile))
        {
            DBG("Modello salvato correttamente.");
        }
        else
        {
            DBG("Errore nel salvataggio del modello.");
        }
    }
    else
    {
        DBG("Percorso del modello non valido.");
    }
}

void NeuralPluginAudioProcessor::loadConfig(File configFile)
{
    this->suspendProcessing(true);
    String path = configFile.getFullPathName();
    char_filename = path.toUTF8();

    try {
        // Load the JSON file into the correct model
        LSTM.load_json(char_filename);

        // Check what the input size is and then update the GUI appropirately
        if (LSTM.input_size == 1) {
            params = 0;
        }
        else if (LSTM.input_size == 2) {
            params = 1;
        }
        else if (LSTM.input_size == 3) {
            params = 2;
        }

        // If we are good: let's say so
        model_loaded = 1;
    }
    catch (const std::exception& e) {
        DBG("Unable to load json file: " + configFile.getFullPathName());
        std::cout << e.what();
    }

    this->suspendProcessing(false);
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NeuralPluginAudioProcessor();
}





