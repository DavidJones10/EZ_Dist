/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <sstream>
#include <iomanip>
//==============================================================================
EZ_DistAudioProcessorEditor::EZ_DistAudioProcessorEditor (EZ_DistAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setLookAndFeel(&otherLookAndFeel);
    setSize (800, 585);
    Timer::startTimerHz(20);
    
    dist_slider.setLookAndFeel(&otherLookAndFeel);
    dist_tone_slider.setLookAndFeel(&otherLookAndFeel);
    dist_mix_slider.setLookAndFeel(&otherLookAndFeel);
    dist_bypassed_button.setLookAndFeel(&otherLookAndFeel);
    
    drive_slider.setLookAndFeel(&otherLookAndFeel);
    drive_tone_slider.setLookAndFeel(&otherLookAndFeel);
    drive_mix_slider.setLookAndFeel(&otherLookAndFeel);
    drive_bypassed_button.setLookAndFeel(&otherLookAndFeel);
    
    gain_pre_slider.setLookAndFeel(&otherLookAndFeel);
    gain_post_slider.setLookAndFeel(&otherLookAndFeel);
    post_lpf_slider.setLookAndFeel(&otherLookAndFeel);
    
    flip_chain_button.setLookAndFeel(&otherLookAndFeel);
    
    addAndMakeVisible(dist_bypassed_button);
    addAndMakeVisible(drive_bypassed_button);
    addAndMakeVisible(flip_chain_button);
    
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    dist_attach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "DIST", dist_slider);
    dist_tone_attach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "DIST_TONE", dist_tone_slider);
    dist_mix_attach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "DIST_MIX", dist_mix_slider);
    dist_bypassed_attach = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "DIST_BYPASSED", dist_bypassed_button);
    
    drive_attach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "DRIVE", drive_slider);
    drive_tone_attach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "DRIVE_TONE", drive_tone_slider);
    drive_mix_attach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "DRIVE_MIX", drive_mix_slider);
    drive_bypassed_attach = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "DRIVE_BYPASSED", drive_bypassed_button);
    
    lpf_attach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "POST_LPF", post_lpf_slider);
    gain_post_attach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "POST_GAIN", gain_post_slider);
    gain_pre_attach =std::make_unique<SliderAttachment>(audioProcessor.apvts, "PRE_GAIN", gain_pre_slider);
    flip_chain_attach = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "DRIVE_LAST", flip_chain_button);
    
    setSliderParametersDial(dist_slider);
    setSliderParametersDial(dist_tone_slider);
    setSliderParametersDial(dist_mix_slider);
    setSliderParametersDial(drive_slider);
    setSliderParametersDial(drive_tone_slider);
    setSliderParametersDial(drive_mix_slider);
    setSliderParametersDial(post_lpf_slider);
    setSliderParametersDial(gain_post_slider);
    setSliderParametersDial(gain_pre_slider);
    
    float startAng = 4 * pi / 3; float endAng = 8 * pi /3;
    dist_slider.setRotaryParameters(startAng, endAng, true);
    dist_tone_slider.setRotaryParameters(startAng, endAng, true);
    dist_mix_slider.setRotaryParameters(startAng, endAng, true);
    
    drive_slider.setRotaryParameters(startAng, endAng, true);
    drive_tone_slider.setRotaryParameters(startAng, endAng, true);
    drive_mix_slider.setRotaryParameters(startAng, endAng, true);
    
    post_lpf_slider.setRotaryParameters(startAng, endAng, true);
    gain_post_slider.setRotaryParameters(startAng, endAng, true);
    gain_pre_slider.setRotaryParameters(startAng, endAng, true);
     
    //distComponent.init(p, "DIST", "DIST_TONE","DIST_MIX","DIST_BYPASSED");
}

EZ_DistAudioProcessorEditor::~EZ_DistAudioProcessorEditor()
{
    setLookAndFeel((nullptr));
    dist_slider.setLookAndFeel(nullptr);
    dist_tone_slider.setLookAndFeel(nullptr);
    dist_mix_slider.setLookAndFeel(nullptr);
    dist_bypassed_button.setLookAndFeel(nullptr);
    
    drive_slider.setLookAndFeel(nullptr);
    drive_tone_slider.setLookAndFeel(nullptr);
    drive_mix_slider.setLookAndFeel(nullptr);
    drive_bypassed_button.setLookAndFeel(nullptr);
    
    gain_pre_slider.setLookAndFeel(nullptr);
    gain_post_slider.setLookAndFeel(nullptr);
    post_lpf_slider.setLookAndFeel(nullptr);
    
    flip_chain_button.setLookAndFeel(nullptr);
}

//==============================================================================
void EZ_DistAudioProcessorEditor::paint (juce::Graphics& g)
{
    dist_image = juce::ImageCache::getFromMemory(BinaryData::EZ_Distortion_background_png,BinaryData::EZ_Distortion_background_pngSize);
    drive_image= juce::ImageCache::getFromMemory(BinaryData::EZ_Overdrive_background_png,BinaryData::EZ_Overdrive_background_pngSize);
    auto dist_pos = dist_slider.getValue()/ dist_slider.getMaximum();
    auto dist_tone_pos = dist_tone_slider.getValue()/ dist_tone_slider.getMaximum();
    auto dist_mix_pos = dist_mix_slider.getValue()/ dist_mix_slider.getMaximum();
   
    auto drive_pos = drive_slider.getValue()/ drive_slider.getMaximum();
    auto drive_tone_pos = drive_tone_slider.getValue()/ drive_tone_slider.getMaximum();
    auto drive_mix_pos = drive_mix_slider.getValue()/ drive_mix_slider.getMaximum();
    
    auto lpf_pos = post_lpf_slider.getValue()/ post_lpf_slider.getMaximum();
    auto pre_gain_pos = gain_pre_slider.getValue()/ gain_pre_slider.getMaximum();
    auto post_gain_pos = fmap(gain_post_slider.getValue(), -20.0, 20.0, 0.0, 1.0);
    
    distCorner = corner1 + corner_gap;
    driveCorner = corner1;
    if (flip_chain_button.getToggleState()){
        distCorner = corner1;
        driveCorner = corner1 + corner_gap;
    }
    
    g.fillAll (Colours::skyblue);
    g.drawImageWithin(dist_image,distCorner-200,-20,600,600,RectanglePlacement::stretchToFit);
    g.drawImageWithin(drive_image,driveCorner-200,-20, 600,600,RectanglePlacement::stretchToFit);
    
    drawRotarySlider(g, distCorner, 35.f, sliderWidth, sliderHeight, dist_pos, 4 * pi / 3, 8 * pi /3, dist_slider,String(""));
    drawRotarySlider(g, distCorner+slider_gap, 38.f, sliderWidth, sliderHeight, dist_tone_pos, 4 * pi / 3, 8 * pi /3, dist_tone_slider,String(""));
    drawRotarySlider(g, distCorner+slider_gap_mid, 150.f, sliderWidth, sliderHeight, dist_mix_pos, 4 * pi / 3, 8 * pi /3, dist_mix_slider,String(""));
    drawToggleButton(g,dist_bypassed_button,distCorner+slider_gap_mid+13.f);
    
    drawRotarySlider(g, driveCorner, 35.f, sliderWidth, sliderHeight, drive_pos, 4 * pi / 3, 8 * pi /3, drive_slider,String(""));
    drawRotarySlider(g, driveCorner+slider_gap, 38.f, sliderWidth, sliderHeight, drive_tone_pos, 4 * pi / 3, 8 * pi /3, drive_tone_slider,String(""));
    drawRotarySlider(g, driveCorner+slider_gap_mid, 150.f, sliderWidth, sliderHeight, drive_mix_pos, 4 * pi / 3, 8 * pi /3, drive_mix_slider,String(""));
    drawToggleButton(g,drive_bypassed_button,driveCorner+slider_gap_mid+13.f);
    
    g.setColour(Colours::grey);
    g.fillRoundedRectangle(615, 17, 170, 545, 10);
    g.setColour(Colours::black);
    g.drawRoundedRectangle(615, 17, 170, 545, 10,5);
    
    drawRotarySlider(g, 650, 82, sliderWidth, sliderHeight, pre_gain_pos, 4 * pi / 3, 8 * pi /3, gain_pre_slider,String("Pre Gain"));
    drawRotarySlider(g, 650, 212, sliderWidth, sliderHeight, post_gain_pos, 4 * pi / 3, 8 * pi /3, gain_post_slider,String("Post Gain"));
    drawRotarySlider(g, 650, 342, sliderWidth, sliderHeight, lpf_pos, 4 * pi / 3, 8 * pi /3, post_lpf_slider,String("Post LPF"));
    
    drawStaticToggleButton(g, flip_chain_button);
    drawParamText(g);
    resized();
    //distComponent.paint(g);
}

void EZ_DistAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    dist_slider.setBounds(distCorner, 35.f, sliderWidth, sliderHeight);
    dist_tone_slider.setBounds(distCorner+slider_gap, 38.f, sliderWidth, sliderHeight);
    dist_mix_slider.setBounds(distCorner+slider_gap_mid, 150.f, sliderWidth, sliderHeight);
    dist_bypassed_button.setBounds(distCorner, 340.f,240.f,185.f);
    
    drive_slider.setBounds(driveCorner, 35.f, sliderWidth, sliderHeight);
    drive_tone_slider.setBounds(driveCorner+slider_gap, 38.f, sliderWidth, sliderHeight);
    drive_mix_slider.setBounds(driveCorner+slider_gap_mid, 150.f, sliderWidth, sliderHeight);
    drive_bypassed_button.setBounds(driveCorner, 340.f,240.f,185.f);
    
    gain_pre_slider.setBounds(650, 82, sliderWidth, sliderHeight);
    gain_post_slider.setBounds(650, 212, sliderWidth, sliderHeight);
    post_lpf_slider.setBounds(650, 342, sliderWidth, sliderHeight);
    
    flip_chain_button.setBounds(625, 470, 150, 80);
     
    //distComponent.resized();
}
void EZ_DistAudioProcessorEditor::timerCallback()
{
    repaint();
}

void EZ_DistAudioProcessorEditor::drawParamText(Graphics& g) {
    std::stringstream stream;
    auto text = String("");

    if (drive_slider.isMouseOverOrDragging()) {
        auto& drive_ = *audioProcessor.apvts.getRawParameterValue("DRIVE");
        stream << "Drive:     " << std::fixed << std::setprecision(2) << drive_;
        text = String(stream.str());
    } else if (drive_tone_slider.isMouseOverOrDragging()) {
        auto& drive_tone = *audioProcessor.apvts.getRawParameterValue("DRIVE_TONE");
        stream << "Drive Tone:  " << std::fixed << std::setprecision(2) << drive_tone;
        text = String(stream.str());
    } else if (drive_mix_slider.isMouseOverOrDragging()) {
        auto& drive_mix = *audioProcessor.apvts.getRawParameterValue("DRIVE_MIX");
        stream << "Drive Mix: " << std::fixed << std::setprecision(2) << drive_mix;
        text = String(stream.str());
    } else if (dist_slider.isMouseOverOrDragging()) {
        auto& dist_ = *audioProcessor.apvts.getRawParameterValue("DIST");
        stream << "Dist:     " << std::fixed << std::setprecision(2) << dist_;
        text = String(stream.str());
    } else if (dist_tone_slider.isMouseOverOrDragging()) {
        auto& dist_tone = *audioProcessor.apvts.getRawParameterValue("DIST_TONE");
        stream << "Dist Tone:  " << std::fixed << std::setprecision(2) << dist_tone;
        text = String(stream.str());
    } else if (dist_mix_slider.isMouseOverOrDragging()) {
        auto& dist_mix = *audioProcessor.apvts.getRawParameterValue("DIST_MIX");
        stream << "Dist Mix: " << std::fixed << std::setprecision(2) << dist_mix;
        text = String(stream.str());
    } else if (post_lpf_slider.isMouseOverOrDragging()) {
        auto& lpf = *audioProcessor.apvts.getRawParameterValue("POST_LPF");
        stream << "Post Lowpass Freq: " << std::fixed << std::setprecision(2) << lpf << "Hz";
        text = String(stream.str());
    } else if (gain_post_slider.isMouseOverOrDragging()) {
        auto& post_gain = *audioProcessor.apvts.getRawParameterValue("POST_GAIN");
        stream << "Post Gain: " << std::fixed << std::setprecision(2) << post_gain << "dB";
        text = String(stream.str());
    } else if (gain_pre_slider.isMouseOverOrDragging()) {
        auto& pre_gain = *audioProcessor.apvts.getRawParameterValue("PRE_GAIN");
        stream << "Pre Gain: " << std::fixed << std::setprecision(2) << pre_gain << "dB";
        text = String(stream.str());
    }

    auto textRect = Rectangle<float>(620, 22, 160, 50);
    g.setFont(Font("Arial", 24.0f, Font::bold));
    g.setColour(Colours::white);
    g.drawFittedText(text, 623, 22, 150, 50, Justification::centredTop, 2);
    g.drawRoundedRectangle(textRect, 10, 2);
    g.setColour(Colours::grey);
    g.setOpacity(0.5);
    g.drawRoundedRectangle(textRect, 10, 2);
}
