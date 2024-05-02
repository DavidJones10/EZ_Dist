/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EZ_DistAudioProcessor::EZ_DistAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
apvts(*this, nullptr, "Parameters",
{
std::make_unique<AudioParameterFloat>(ParameterID("DIST",1), "Dist", NormalisableRange<float> { 0.0f, 1.0f, .001f }, 0.5f),
std::make_unique<AudioParameterFloat>(ParameterID("DIST_TONE",1), "Dist Tone", NormalisableRange<float> { 0.0f, 1.0f, .001f }, 0.5f),
std::make_unique<AudioParameterFloat>(ParameterID("DIST_MIX",1), "Dist Mix", NormalisableRange<float> { 0.0f, 1.0f, .001f }, 0.5f),
std::make_unique<AudioParameterBool>(ParameterID("DIST_BYPASSED",1), "Dist Bypassed", false),
std::make_unique<AudioParameterFloat>(ParameterID("DRIVE",1), "Drive", NormalisableRange<float> { 0.f, 1.0f, .001f }, 0.5f),
std::make_unique<AudioParameterFloat>(ParameterID("DRIVE_TONE",1), "Drive Tone", NormalisableRange<float> { 0.0f, 1.0f, .001f }, 0.5f),
std::make_unique<AudioParameterFloat>(ParameterID("DRIVE_MIX",1), "Drive Mix", NormalisableRange<float> { 0.0f, 1.0f, .001f }, 0.5f),
std::make_unique<AudioParameterBool>(ParameterID("DRIVE_BYPASSED",1), "Drive Bypassed", false),
std::make_unique<AudioParameterBool>(ParameterID("DRIVE_LAST",1), "Drive Last", false), // process order for drive and distortion
std::make_unique<AudioParameterFloat>(ParameterID("POST_LPF",1), "Post Lowpass Frequency", NormalisableRange<float> { 150.0f, 10000.0f, 1.f }, 5000.f),
std::make_unique<AudioParameterFloat>(ParameterID("POST_GAIN",1), "Post Gain", NormalisableRange<float> { -20.f, 20.f, .01f }, 0.f),
std::make_unique<AudioParameterFloat>(ParameterID("PRE_GAIN",1), "Pre Gain", NormalisableRange<float> { 0.f, 15.f, .01f }, 0.f),

}),
oversampling(2, 4, dsp::Oversampling<float>::filterHalfBandFIREquiripple)
#endif
{
}

EZ_DistAudioProcessor::~EZ_DistAudioProcessor()
{
}

//==============================================================================
const juce::String EZ_DistAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EZ_DistAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EZ_DistAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EZ_DistAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EZ_DistAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EZ_DistAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EZ_DistAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EZ_DistAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EZ_DistAudioProcessor::getProgramName (int index)
{
    return {};
}

void EZ_DistAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EZ_DistAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oversampling.initProcessing(samplesPerBlock);
    drive[0].init(sampleRate);
    drive[1].init(sampleRate);
    dist[0].init(sampleRate);
    dist[1].init(sampleRate);
    lpf[0].init(sampleRate);
    lpf[1].init(sampleRate);
    
}

void EZ_DistAudioProcessor::releaseResources()
{
    oversampling.reset();
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EZ_DistAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void EZ_DistAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    auto& dist_ = *apvts.getRawParameterValue("DIST");
    auto& dist_tone = *apvts.getRawParameterValue("DIST_TONE");
    auto& dist_mix = *apvts.getRawParameterValue("DIST_MIX");
    auto& dist_bypassed = *apvts.getRawParameterValue("DIST_BYPASSED");
    
    auto& drive_ = *apvts.getRawParameterValue("DRIVE");
    auto& drive_tone = *apvts.getRawParameterValue("DRIVE_TONE");
    auto& drive_mix = *apvts.getRawParameterValue("DRIVE_MIX");
    auto& drive_bypassed = *apvts.getRawParameterValue("DRIVE_BYPASSED");
    
    auto& drive_last = *apvts.getRawParameterValue("DRIVE_LAST"); // if true, drive will be processed after distortion

    auto& post_lpf = *apvts.getRawParameterValue("POST_LPF");
    auto& post_gain = *apvts.getRawParameterValue("POST_GAIN");
    auto& pre_gain = *apvts.getRawParameterValue("PRE_GAIN");
    
    dist[0].set_params(dist_, dist_tone, dist_mix, dist_bypassed);
    dist[1].set_params(dist_, dist_tone, dist_mix, dist_bypassed);
    drive[0].set_params(drive_, drive_tone, drive_mix, drive_bypassed);
    drive[1].set_params(drive_, drive_tone, drive_mix, drive_bypassed);
    lpf[0].set_params(post_lpf*.1f,1.f);
    lpf[1].set_params(post_lpf*.1f,1.f);
    

    dsp::AudioBlock<float> block (buffer);
    dsp::AudioBlock<float> osBlock = oversampling.processSamplesUp (block);
    float* ptrArray[] = {osBlock.getChannelPointer (0), osBlock.getChannelPointer (1)};
    AudioBuffer<float> osBuffer (ptrArray, 2, static_cast<int> (osBlock.getNumSamples()));
    
    for (int channel=0; channel< osBuffer.getNumChannels(); channel++){
        auto* write_samples = osBuffer.getWritePointer(channel);
        
        for (int sample=0; sample < osBuffer.getNumSamples(); sample++){
            float in_val = write_samples[sample]*Decibels::decibelsToGain(static_cast<float>(pre_gain));
            float out_val = 0.f;
            if (drive_last){
                float after_dist = dist[channel].process(in_val);
                out_val = drive[channel].process(after_dist);
            }else{
                float after_drive = drive[channel].process(in_val);
                out_val = dist[channel].process(after_drive);
            }
            write_samples[sample] = lpf[channel].process(out_val)*Decibels::decibelsToGain(static_cast<float>(post_gain));
        }
    }
    oversampling.processSamplesDown (block);
}

//==============================================================================
bool EZ_DistAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EZ_DistAudioProcessor::createEditor()
{
    return new EZ_DistAudioProcessorEditor (*this);
}

//==============================================================================
void EZ_DistAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, destData);
}

void EZ_DistAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

        if (xmlState.get() != nullptr)
            if (xmlState->hasTagName(apvts.state.getType()))
                apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EZ_DistAudioProcessor();
}
