#pragma once
//
//  distortion.h
//  EZ_Dist
//
//  Created by David Jones on 4/13/24.
//

#ifndef distortion_h
#define distortion_h

#include <JuceHeader.h>
#include "my_biquad.h"
#include <math.h>


struct MyDistortion {
public:
    void init(float samplRate);
    float process(float input);
    void set_params(float dist, float filter_freq);
    
private:
    void get_opamp_coefs();
    float diode_clipper(float in_sample);
    float tone_stage(float in_sample);
    float c = 0.0000416666f; // 2/48kHz sample rate
    float c_sqrd = 0.000000001736166f; // ^2;
    MyBiquad bjt_filt;
    MyBiquad op_amp_filt;
    MyBiquad lowpass;
    MyBiquad highpass;
 
    float two_pi = juce::MathConstants<float>::twoPi;
    float w1 = two_pi*3.f; float w2 = two_pi*600.f; // poles of bjt transfer functoin
    float Rt = 100000.f; float Rb = 4700.f + 50000.f; float Cz = 0.000001f; float Cc = 0.000000000250f;
    float dist = 0.5f;
    float tone = 0.5f;
};


void MyDistortion:: init(float sampleRate){
   // coefficients = {b0, b1, b2, a0, a1, a2} // format for iir coefficients

   c = 2 / sampleRate;
    c_sqrd = c * c; 
   
    float bjt_a0 = w1*w2; float bjt_a1 = w1+w2;
    float B0 = c_sqrd;  // B0 = b0 + b1*c + b2*c^2
    float B1 = 2.f*c_sqrd; // B1 = 2*b0 + 2*b2*c^2
    float B2 = c_sqrd; // B2  =b0 - b1*c + a2*c^
    float A0 = bjt_a0 + bjt_a1*c + c_sqrd; // A0 = a0 + a1*c + a2*c^2
    float A1 = 2.f*bjt_a0 + 2*c_sqrd; // A1 = 2*a0 + 2*a2*c^2
    float A2 = bjt_a0 - bjt_a1*c + c_sqrd; // A0  =a0 - a1*c + a2*c^
    bjt_filt.set_coefficients(B0, B1, B2, A0, A1, A2);
    
    float tone_b1 = 6800.f*0.000000022f;
    float tone_a0 = 1.f; float tone_a1 = 9000.f*0.000000022f;
    float tone_A0 = tone_a0 + tone_a1*c;
    float tone_A1 = tone_a0 - tone_a1*c;
    float tone_B0 = tone_b1*c;
    highpass.set_coefficients(tone_B0, tone_B0, 0.0, tone_A0, tone_A1, 0.0);
    
    float tone_a1_lo = 6800.f*0.0000001f;
    float tone_A0_lo = 1.f + tone_a1_lo*c;
    float tone_A1_lo = 1.f - tone_a1_lo*c;
    lowpass.set_coefficients(1.f, 1.f, 0.0, tone_A0_lo, tone_A1_lo, 0.0);
    
    MyDistortion::get_opamp_coefs();
}

void MyDistortion::get_opamp_coefs(){
    Rt = 100000.f * dist;
    Rb = (1-dist) * 100000.f + 4700.f;
    float op_a0 = 1/(Rt*Rb*Cc*Cz); float op_a1 = 1/(Rb*Cz) + 1/(Rt*Cc); float op_a2 = 1.f; 
    float op_b0 =op_a0; float op_b1 =op_a1 + 1.f/(Rb*Cc); float op_b2 = 1.f;
 
    float B0 = op_b0 + op_b1*c + op_b2*c_sqrd;  // B0 = b0 + b1*c + b2*c^2
    float B1 = 2.f*op_b0 - 2.f*op_b2*c_sqrd; // B1 = 2*b0 + 2*b2*c^2
    float B2 = op_b0 - op_b1*c + op_b2*c_sqrd; // B2  =b0 - b1*c + a2*c^
    float A0 = op_a0 + op_a1*c + op_a2*c_sqrd; // A0 = a0 + a1*c + a2*c^2
    float A1 = 2.f*op_a0 - 2.f*op_a2*c_sqrd; // A1 = 2*a0 + 2*a2*c^2
    float A2 = op_a0 - op_a1*c + op_a2*c_sqrd; // A0  =a0 - a1*c + a2*c^
    
    op_amp_filt.set_coefficients(B0, B1, B2, A0, A1, A2);
}

float MyDistortion::diode_clipper(float in_sample){
    float divisor = powf((1+ powf(fabsf(in_sample), 2.5)), 0.4); // (1+|x|^n)^(1/n) n=2.5
    return in_sample / divisor;
}

float MyDistortion::process(float input){
    float after_bjt = bjt_filt.process(input);
    float after_opamp = op_amp_filt.process(after_bjt);
    float after_clipper = MyDistortion::diode_clipper(after_opamp);
    float after_tone = lowpass.process(after_clipper)*tone + highpass.process(after_clipper)*(1-tone);
    return after_tone;
}

void MyDistortion::set_params(float dist_, float tone_){
    dist = dist_ >= 1.0 ? 1.0 : dist_;
    dist = dist_ <= 1.0 ? 0.0 : dist_;
    tone = tone_ >= 1.0 ? 1.0 : tone_;
    tone = tone_ <= 1.0 ? 0.0 : tone_;
    MyDistortion::get_opamp_coefs();
}

#endif /* distortionh */

