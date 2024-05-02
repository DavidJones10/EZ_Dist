#pragma once
//
//  distortion.h
//  EZ_Dist
//
//  Created by David Jones on 4/13/24.
//

#ifndef my_distortion_h
#define my_distortion_h

#include "my_biquad.h"
#include <math.h>
#include <iostream>


struct MyDistortion {
public:
    void init(float sampleRate){
        // coefficients = {b0, b1, b2, a0, a1, a2} // format for iir coefficients
        bjt_filt.init();
        op_amp_filt.init();
        lowpass.init();
        highpass.init();
        c = 2.f * sampleRate;
        c_sqrd = c * c;
        // Calculate coefficients for bjt stage
        float bjt_a0 = w1*w2; float bjt_a1 = w1+w2;
        float B0 = c_sqrd;  // B0 = b0 + b1*c + b2*c^2
        float B1 = 2.f*c_sqrd; // B1 = 2*b0 + 2*b2*c^2
        float B2 = c_sqrd; // B2  =b0 - b1*c + a2*c^
        float A0 = bjt_a0 + bjt_a1*c + c_sqrd; // A0 = a0 + a1*c + a2*c^2
        float A0_inv = 1.f/A0;
        float A1 = 2.f*bjt_a0 + 2.f*c_sqrd; // A1 = 2*a0 + 2*a2*c^2
        float A2 = bjt_a0 - bjt_a1*c + c_sqrd; // A0  =a0 - a1*c + a2*c^
        bjt_filt.set_coefficients(B0*A0_inv, B1*A0_inv, B2*A0_inv, 1.f, A1*A0_inv, A2*A0_inv);
        //DBG("BJT Coefficients:   B0 : "<<B0*A0_inv<<" B1: "<<B1*A0_inv<<" B2: "<<B2*A0_inv<<" A0: "<<A0<<" A1: "<<A1*A0_inv<<" A2: "<<A2*A0_inv);
         // Calculate coefficients for highpass block of tone stage
        float tone_b1 = 6800.f*0.000000022f;
        float tone_a0 = 1.f; float tone_a1 = 9000.f*0.000000022f;
        float tone_A0 = tone_a0 + tone_a1*c;
        float tone_A0_inv = 1.f/tone_A0;
        float tone_A1 = 2*tone_a0;
        float tone_A2 = tone_a0 - tone_a1*c;
        float tone_B0 = tone_b1*c;
        float tone_B2 = -tone_b1*c;
        highpass.set_coefficients(tone_B0*tone_A0_inv, 0.f, tone_B2*tone_A0_inv, 1.f, tone_A1*tone_A0_inv, tone_A2*tone_A0_inv);
        //DBG("Highpass Coefficients:   B0: "<<tone_B0*tone_A0_inv<<" B1: "<<tone_B0*tone_A0_inv<<" B2: "<<0.0f<<" A0: "<<tone_A0<<" A1: "<<tone_A1*tone_A0_inv<<" A2: "<<0.0f);
        // Calculate coefficients for lowpass block of tone stage
        float tone_a1_lo = 6800.f*0.0000001f;
        float tone_A0_lo = 1.f + tone_a1_lo*c;
        float tone_lo_inv = 1.f / tone_A0_lo;
        float tone_A1_lo = 2.f;
        float tone_A2_lo = 1.f - tone_a1_lo*c;
        //tone_B0_lo = 1.f;
        //tone_B1_lo = 2.f;
        //tone_B2_lo = 1.f;
        lowpass.set_coefficients(1.f*tone_lo_inv, 2.f*tone_lo_inv, 1.0f*tone_lo_inv, 1.f, tone_A1_lo*tone_lo_inv, tone_A2_lo*tone_lo_inv);
        //DBG("Lowpass Coefficients:   B0: "<<1.f*tone_lo_inv<<" B1: "<<1.f*tone_lo_inv<<" B2: "<<0.0f<<" A0: "<<tone_A0_lo<<" A1: "<<tone_A1_lo*tone_lo_inv<<" A2: "<<0.0f);
         get_opamp_coefs();
     };
    
    float process(float input){
        if (bypassed){
            return input;
        }
        //input *= .9f;
        float after_bjt = bjt_filt.process(input);
        float after_opamp = op_amp_filt.process(after_bjt);
        float after_clipper = diode_clipper(after_opamp);
        float after_tone = lowpass.process(after_clipper)*(1.f-tone) + highpass.process(after_clipper)*tone;
        return input*(1.f-mix) + after_tone*mix;
    };
    
    void set_params(float dist_, float tone_, float mix_, bool bypassed_){
        dist = fclamp(dist_,.001f,1.f);
        tone = fclamp(tone_,0.f,1.f);
        mix = fclamp(mix_,0.f,1.f);
        bypassed = bypassed_;
        if (!bypassed){
            get_opamp_coefs();
        }
    };
    
private:
    void get_opamp_coefs(){
        Rt = 100000.f * dist;
        Rb = (1.f-dist) * 100000.f + 4700.f;
        float op_a0 = 1.f/(Rt*Rb*Cc*Cz); float op_a1 = 1.f/(Rb*Cz) + 1.f/(Rt*Cc); float op_a2 = 1.f;
        float op_b0 =op_a0; float op_b1 =op_a1 + 1.f/(Rb*Cc); float op_b2 = 1.f;
     
        float B0 = op_b0 + op_b1*c + op_b2*c_sqrd;  // B0 = b0 + b1*c + b2*c^2
        float B1 = 2.f*op_b0 - 2.f*op_b2*c_sqrd; // B1 = 2*b0 + 2*b2*c^2
        float B2 = op_b0 - op_b1*c + op_b2*c_sqrd; // B2  =b0 - b1*c + a2*c^
        float A0 = op_a0 + op_a1*c + op_a2*c_sqrd; // A0 = a0 + a1*c + a2*c^2
        float A0_inv = 1.f / A0;
        float A1 = 2.f*op_a0 - 2.f*op_a2*c_sqrd; // A1 = 2*a0 - 2*a2*c^2
        float A2 = op_a0 - op_a1*c + op_a2*c_sqrd; // A2  =a0 - a1*c + a2*c^2
        
        op_amp_filt.set_coefficients(B0*A0_inv, B1*A0_inv, B2*A0_inv, 1.f, A1*A0_inv, A2*A0_inv);
        //DBG("op_amp Coefficients:   B0 : "<<B0*A0_inv<<" B1: "<<B1*A0_inv<<" B2: "<<B2*A0_inv<<" A0: "<<A0<<" A1: "<<A1*A0_inv<<" A2: "<<A2*A0_inv);
    };
    float diode_clipper(float in_sample){
        float divisor = powf((1.f+ powf(fabsf(in_sample), 2.5)), 0.4f); // (1+|x|^n)^(1/n) n=2.5
        return in_sample / divisor;
    };
    
    float fclamp(float input, float low_rng, float high_rng){
        if (input >= high_rng){
            return high_rng;
        }else if (input <= low_rng){
            return low_rng;
        }else{
            return input;
        }
    }
    float c = 96000; // 2/48kHz sample rate
    float c_sqrd = 9216000000; // ^2;
    MyBiquad bjt_filt;
    MyBiquad op_amp_filt;
    MyBiquad lowpass;
    MyBiquad highpass;
 
    float two_pi = juce::MathConstants<float>::twoPi;
    float w1 = two_pi*3.f; float w2 = two_pi*600.f; // poles of bjt transfer functoin
    float Rt = 100000.f; float Rb = 4700.f + 50000.f; float Cz = 0.000001f; float Cc = 0.000000000250f;
    float dist = 0.5f;
    float tone = 0.5f;
    float mix = 0.5f;
    bool bypassed = false;
};

#endif /* my_distortion_h */
