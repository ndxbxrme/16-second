cmake_minimum_required(VERSION 3.16)

project(16Second VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Workaround for GCC 13 + JUCE 6.1.6 missing <utility> include in juceaide.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include utility")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:-include;utility>")

include(FetchContent)

# JUCE
FetchContent_Declare(
  juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 6.1.6
)
FetchContent_MakeAvailable(juce)

# Catch2 for tests
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(catch2)

juce_add_plugin(16Second
  COMPANY_NAME "Kieron"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
  PLUGIN_MANUFACTURER_CODE Kiro
  PLUGIN_CODE SxSx
  FORMATS VST3
  PRODUCT_NAME "16-Second"
)

juce_generate_juce_header(16Second)

target_sources(16Second
  PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/dsp/MemoryBuffer.cpp
    Source/dsp/MemoryBuffer.h
    Source/dsp/StateMachine.cpp
    Source/dsp/StateMachine.h
    Source/dsp/Overdub.cpp
    Source/dsp/Overdub.h
    Source/dsp/RateStepper.cpp
    Source/dsp/RateStepper.h
    Source/dsp/Smoother.cpp
    Source/dsp/Smoother.h
    Source/dsp/FeedbackModel.cpp
    Source/dsp/FeedbackModel.h
    Source/dsp/Limiter.cpp
    Source/dsp/Limiter.h
)

target_compile_definitions(16Second
  PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(16Second
  PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

enable_testing()
add_subdirectory(tests)
