/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class EZ_DistAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    EZ_DistAudioProcessorEditor (EZ_DistAudioProcessor&);
    ~EZ_DistAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EZ_DistAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EZ_DistAudioProcessorEditor)
};
