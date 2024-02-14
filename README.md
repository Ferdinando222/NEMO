# NEMO

This is the NEMO repository, which is a plugin utilizing neural networks to emulate the sound of analog guitar pedals like distortions, fuzz, and overdrive.

## THE PROJECT'S OBJECTIVE

The primary aim is to develop a user-friendly plugin that enables users to accurately reproduce the sound and behavior of non-time-varying analog effect pedals, such as distortion or overdrive. This entails providing musicians and audio enthusiasts with a seamless digital alternative that faithfully emulates the unique characteristics and nuances of these classic analog devices. The objective is to ensure that users can easily integrate the plugin into their workflow, harnessing its capabilities to enhance their musical compositions or recordings while preserving the authentic tone and dynamics of traditional analog equipment.

## HOW RUN NEMO PLUGIN
You can follow this tutorial to run the application on your own machine.
### Step 0 - Copy the Repo
To run the various components of the application, you first need to download the github repo on your local machine.
### Step - Modify the CMAkeLists.txt
In the CMakeLists.txt, modify the path "add_subdirectory("C:/Users/Utente/OneDrive - Politecnico di Milano/Desktop/JUCE" ./JUCE)" with your local path associated with JUCE.
### Step 2 - Build with Cmake
Stay in the repository's home directory and create a build for Visual Studio using CMake
### Step 3 - Open the project in Visual Studio
Open the project in Visual Studio and build the project. `For safety, rebuild with CMake if Visual Studio cannot find files such as JuceHeader.h.`
### Step 4 - Now you can run the NEMO plugin
You can run the NEMO plugin as a standalone software or in a DAW as a VST.

<a align="center" href="https://github.com/Ferdinando222/NEMO" target="_blank">
  <img src="./Documentation/nemo.png" width="50%" />
</a>


## HOW RUN PYTHON SCRIPT FOR TRAINING

### Requiriements
-You need to have an account on Weights and Biases in order to monitor the training.
-Python >= 3.9
### Step 0 - Copy the Repo and create a python environment
 You first need to download the github repo on your local machine. Create a python environment using the requirements.txt.
### Step 1 - Login in wandb
Log in to your Weights and Biases account using the command wandb login and enter your API key when prompted.
### Step 2 - Create a folder data
Create a data folder with this structure
  
    data
    ├── input
    │   ├── test
    │   │  ├──guitar.wav
    │   ├── train
    │   │   ├──clean_signal-001.wav
    ├── output
    │   ├── test
    │   │   ├── muff_test.wav
    │   ├── train
    │   │   ├── muff_train.wav
### Step 3 - Run the train.py
Run the python train.py script

## TESTING FOLDER
The "testing" folder contains scripts used to assess the performance of the plugin. To test the plugin in your own folder, follow this brief tutorial.
### Step 0 - Copy the Repo
To run the various components of the application, you first need to download the github repo on your local machine.
### Step - Modify the CMAkeLists.txt
In the CMakeLists.txt, update the path specified in the line set(CMAKE_PREFIX_PATH "C:/libtorch") with the path where you have the libtorch folder.
### Step 2 - Build with Cmake
Stay in the Test directory and create a build for Visual Studio using CMake
### Step 3 - Open the project in Visual Studio
Open the project in Visual Studio and build the project. `For safety, rebuild with CMake if Visual Studio cannot find files such as JuceHeader.h.`
### Step 4 - Now choose the model that you want to test and run the code
The output will be printed in the build folder in the output.txt file. It will display the elapsed time for each model.
## OTHER INFO
For more info about Nemo, take a look at the official paper, where both the plugin and its underlying neural network model are described in detail.
