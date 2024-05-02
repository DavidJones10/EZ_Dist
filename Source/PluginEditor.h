/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
//#include "pedal_gui.h"

//==============================================================================
/**
*/

struct OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OtherLookAndFeel()
    {
        setColour (juce::Slider::thumbColourId, juce::Colours::darkblue);
    }
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider, const String& label)
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();
 
        // fill
        g.setColour (juce::Colours::black);
        g.fillEllipse (rx, ry, rw, rw);
        
        // outline
        g.setColour (juce::Colours::darkblue);
        g.drawEllipse (rx, ry, rw, rw, 1.0f);
        
        if (slider.isEnabled())
            g.setColour (slider.findColour (Slider::rotarySliderFillColourId).withAlpha (isMouseOver ? 1.0f : 0.7f));
        else
            g.setColour (Colour (0x80808080));
        
        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 2.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
        // pointer
        g.setColour (juce::Colours::white);
        g.fillPath (p);
        // textbox
        g.setColour(Colours::black);
        g.setFont(24.0f);
        g.drawFittedText(label , x, slider.getBottom() - 3, width, 25, Justification::centred, 3);
        g.setColour(Colours::black);
    }
    
    void drawToggleButton(Graphics& g, ToggleButton& button,float led_x){
        if (!button.getToggleState()) // If toggle is active
            {
                g.setColour(Colours::red.withAlpha(0.5f)); // Set color to red
                
                g.fillEllipse(led_x,367.f,70.f,70.f);
            }
    }
    void drawToggleButton(Graphics& g, ToggleButton&,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown){
        
    }// Do nothing, just to override the original
    void drawStaticToggleButton(Graphics& g, ToggleButton& button){
        // Set colors
        Colour fillColor = button.getToggleState() ? Colour(0xffd8b7ff) : Colour(0xfff5e6c4);
        Colour textColor = button.getToggleState() ? Colours::black : Colours::black;

        // Draw rounded rectangle
        g.setColour(fillColor);
        g.fillRoundedRectangle(625, 470, 150, 80, 10);
        g.setColour(Colours::black);
        g.drawRoundedRectangle(625, 470, 150, 80, 10,8);

        // Draw text
        g.setColour(textColor);
        g.setFont(Font("Impact", 20.0f, Font::bold));
        g.drawText("Press To Flip Chain",625, 480, 150, 80, Justification::centredTop);

        String text1, text2;
        if (button.getToggleState()) {
            text1 = "1. Distortion";
            text2 = "2. Overdrive";
        } else {
            text1 = "1. Overdrive";
            text2 = "2. Distortion";
        }

        auto textBounds1 = Rectangle<int>(628,495,150,40);
        auto textBounds2 = Rectangle<int>(628,515,150,40);

        g.drawText(text1, textBounds1, Justification::centred);
        g.drawText(text2, textBounds2, Justification::centred);
    }
};
 
//public OtherLookAndFeel
class EZ_DistAudioProcessorEditor  : public juce::AudioProcessorEditor, public Timer, public OtherLookAndFeel
{
public:
    EZ_DistAudioProcessorEditor (EZ_DistAudioProcessor&);
    ~EZ_DistAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
    void setSliderParametersDial (Slider& slider, bool hasTextBox = false)
        {
            slider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
            if (hasTextBox)
                slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
            else
                slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 50, 25);
            addAndMakeVisible(slider);
            slider.setAlpha(0);
            slider.setLookAndFeel(&otherLookAndFeel);
        }
    void drawParamText(Graphics& g);
    float fmap(float value, float inMin, float inMax, float outMin, float outMax) {
        return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    }
    
     
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EZ_DistAudioProcessor& audioProcessor;
    
    Slider dist_slider;
    Slider dist_tone_slider;
    Slider dist_mix_slider;
    ToggleButton dist_bypassed_button;
    
    Slider drive_slider;
    Slider drive_tone_slider;
    Slider drive_mix_slider;
    ToggleButton drive_bypassed_button;
    
    Slider gain_pre_slider;
    Slider gain_post_slider;
    Slider post_lpf_slider;
    ToggleButton flip_chain_button;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dist_attach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dist_tone_attach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dist_mix_attach;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> dist_bypassed_attach;

    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> drive_attach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> drive_tone_attach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> drive_mix_attach;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> drive_bypassed_attach;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gain_pre_attach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gain_post_attach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lpf_attach;
    
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> flip_chain_attach;
    OtherLookAndFeel otherLookAndFeel;
    const float pi = MathConstants<const float>::pi;
    float sliderWidth = 100.f; float sliderHeight = 100.f;
    float corner1 = 40.f; float corner_gap = 300.f;
    float slider_gap = 140.f; float slider_gap_mid = 75.f;
    float distCorner = corner1+corner_gap; float driveCorner = corner1;
    juce::Image dist_image;
    juce::Image drive_image;
    
    //GuitarPedalComponent distComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EZ_DistAudioProcessorEditor)
};


