

cmake_minimum_required(VERSION 3.15)


project(NEMO VERSION 0.0.1)

add_subdirectory("C:/Users/Utente/OneDrive - Politecnico di Milano/Desktop/JUCE" ./JUCE)                    # If you've put JUCE in a subdirectory called JUCE
set(RTNEURAL_STL ON CACHE BOOL "Use RTNeural with this backend" FORCE)
add_subdirectory(RTNeural ${CMAKE_CURRENT_BINARY_DIR}/RTNeural)
add_subdirectory(Images)
add_compile_definitions(JUCE_MODAL_LOOPS_PERMITTED=1)
include_directories(RTNeural)

juce_add_plugin(Plugin
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME Yee-King                          # Specify the name of the plugin's author
    IS_SYNTH FALSE                       # Is this a synth or an effect?
    NEEDS_MIDI_INPUT TRUE               # Does the plugin need midi input?
    # NEEDS_MIDI_OUTPUT TRUE/FALSE              # Does the plugin need midi output?
    # IS_MIDI_EFFECT TRUE/FALSE                 # Is this plugin a MIDI effect?
    NEEDS_MIDI_OUTPUT TRUE
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD TRUE        # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Yeek               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Yee3                           # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "Plugin")        # The name of the final executable, which can differ from the target name


juce_generate_juce_header(Plugin)

target_sources(Plugin
    PRIVATE
    Plugin/Plugin/Source/PluginEditor.cpp
    Plugin/Plugin/Source/PluginProcessor.cpp
    Plugin/Plugin/Source/PluginEditor.h
    Plugin/Plugin/Source/PluginProcessor.h 
    Plugin/Plugin/Source/RT_LSTM.h
    Plugin/Plugin/Source/RT_LSTM.cpp 
    Plugin/Plugin/Source/CustomKnob.h
    Plugin/Plugin/Source/CustomKnob.cpp   
    Plugin/Plugin/Source/AmpOSCReceiver.h
    Plugin/Plugin/Source/Delay.h
)




target_compile_definitions(Plugin
    PUBLIC # 
        JUCE_ALSA=1
        JUCE_DIRECTSOUND=1
        JUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1
        JUCE_PLUGINHOST_LADSPA=1
        JUCE_PLUGINHOST_LV2=1
        JUCE_PLUGINHOST_VST3=1
        JUCE_USE_OGGVORBIS=1
        JUCE_VST3_HOST_CROSS_PLATFORM_UID=1
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)


target_link_libraries(Plugin
    PRIVATE
        juce::juce_audio_utils
	juce::juce_dsp
	juce::juce_gui_basics
	juce::juce_osc
    PUBLIC
	MyBinaryData
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)



target_link_libraries(Plugin PUBLIC RTNeural)