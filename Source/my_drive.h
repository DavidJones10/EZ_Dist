/*
  ==============================================================================

    my_drive.h
    Created: 1 May 2024 12:01:47am
    Author:  David Jones

  ==============================================================================
*/

#pragma once

#ifndef my_drive_h
#define my_drive_h

#include "my_biquad.h"

struct MyDrive{
public:
    void init(float sampleRate){
        bypassed_ = false;
        c = 2*sampleRate;
        c_sqrd = c*c;
        float hpf1_b0 = 0.f; float hpf1_b1 = 0.01f; float hpf1_b2 = 0.f;
        float hpf1_a0 = 1.f; float hpf1_a1 = 0.01f; float hpf1_a2 = 0.f;
        float hpf1_A0 = hpf1_a0 + hpf1_a1*c + hpf1_a2*c_sqrd; // A0 = a0 + a1*c + a2*c^2
        float hpf1_inv = 1.f / hpf1_A0;
        float hpf1_A1 = 2*hpf1_a0 - 2*hpf1_a2*c_sqrd;
        float hpf1_A2 = hpf1_a0 - hpf1_a1*c + hpf1_a2*c_sqrd;
        float hpf1_B0 = hpf1_b0 + hpf1_b1*c + hpf1_b2*c_sqrd; // A0 = a0 + a1*c + a2*c^2
        float hpf1_B1 = 2*hpf1_b0 - 2*hpf1_b2*c_sqrd;
        float hpf1_B2 = hpf1_b0 - hpf1_b1*c + hpf1_b2*c_sqrd;
        hpf1.set_coefficients(hpf1_B0*hpf1_inv,hpf1_B1*hpf1_inv,hpf1_B2*hpf1_inv,1.f,hpf1_A1*hpf1_inv,hpf1_A2*hpf1_inv);
        
        
        float hpf2_b0 = 0.f; float hpf2_b1 = 0.0102f; float hpf2_b2 = 0.f;
        float hpf2_a0 = 1.f; float hpf2_a1 = 0.0102f; float hpf2_a2 = 0.f;
        float hpf2_A0 = hpf2_a0 + hpf2_a1*c + hpf2_a2*c_sqrd; // A0 = a0 + a1*c + a2*c^2
        float hpf2_inv = 1.f / hpf2_A0;
        float hpf2_A1 = 2*hpf2_a0 - 2*hpf2_a2*c_sqrd;
        float hpf2_A2 = hpf2_a0 - hpf2_a1*c + hpf2_a2*c_sqrd;
        float hpf2_B0 = hpf2_b0 + hpf2_b1*c + hpf2_b2*c_sqrd; // A0 = a0 + a1*c + a2*c^2
        float hpf2_B1 = 2*hpf2_b0 - 2*hpf2_b2*c_sqrd;
        float hpf2_B2 = hpf2_b0 - hpf2_b1*c + hpf2_b2*c_sqrd;
        hpf2.set_coefficients(hpf2_B0*hpf2_inv,hpf2_B1*hpf2_inv,hpf2_B2*hpf2_inv,1.f,hpf2_A1*hpf2_inv,hpf2_A2*hpf2_inv);
        calculate_coefficients();
    }
    float process(float in_sample){
        if (bypassed_){
            return in_sample;
        }
        float after_hpf1 = hpf1.process(in_sample);
        float after_op_amp = op_amp.process(after_hpf1);
        float after_clip = diode_clipper(after_op_amp);
        float sum_ = after_clip + in_sample;
        float after_tone = toneFilt.process(sum_);
        float after_hpf2 = hpf2.process(after_tone);
        return in_sample*(1.f-mix_) + after_hpf2*mix_;
    }
    void set_params(float drive, float tone, float mix, bool bypassed){
        drive_ = fclamp(drive,0.f,1.f);
        tone_ = fclamp(tone,0.f,1.f);
        mix_ = fclamp(mix,0.f,1.f);
        bypassed_ = bypassed;
        if (!bypassed_){
            calculate_coefficients();
        }
    }
private:
    
    float fclamp(float input, float low_rng, float high_rng){
        if (input >= high_rng){
            return high_rng;
        }else if (input <= low_rng){
            return low_rng;
        }else{
            return input;
        }
    }
    float diode_clipper(float in_sample){
        float divisor = powf((1.f+ powf(fabsf(in_sample), 2.5)), 0.4f); // (1+|x|^n)^(1/n) n=2.5
        return in_sample / divisor;
    };
    void calculate_coefficients(){
        // Calculate coefficients for op_amp gain stage
        float R2 = 51000.f + drive_*500000.f;
        float op_amp_b0 = 0.f; float op_amp_b1 = R2; float op_amp_b2 = 0.f;
        float op_amp_a0 = 21276599.74f;/*R1*Wz*/ float op_amp_a1 = 4700.f; float op_amp_a2 = 0.f;
        float op_amp_A0 = op_amp_a0 + op_amp_a1*c + op_amp_a2*c_sqrd; // A0 = a0 + a1*c + a2*c^2
        float op_amp_inv = 1.f / op_amp_A0;
        float op_amp_A1 = 2*op_amp_a0 - 2*op_amp_a2*c_sqrd;
        float op_amp_A2 = op_amp_a0 - op_amp_a1*c + op_amp_a2*c_sqrd;
        float op_amp_B0 = op_amp_b0 + op_amp_b1*c + op_amp_b2*c_sqrd; // A0 = a0 + a1*c + a2*c^2
        float op_amp_B1 = 2*op_amp_b0 - 2*op_amp_b2*c_sqrd;
        float op_amp_B2 = op_amp_b0 - op_amp_b1*c + op_amp_b2*c_sqrd;
        op_amp.set_coefficients(op_amp_B0*op_amp_inv,op_amp_B1*op_amp_inv,op_amp_B2*op_amp_inv,1.f,op_amp_A1*op_amp_inv,op_amp_A2*op_amp_inv);
        
        // Calculate Coefficients for Tone Filter
        float Rl = tone_*20000.f; float Rr = (1-tone_)*20000.f;
        float Rz_plus = Rz + Rl*Rr/(Rl+Rr); // Rz + Rl || Rr
        float Wz = 1.f / (Czs*Rz_plus);
        float Y = Rz_plus * (Rl + Rr);
        float X = Rr / (Y*Czs);
        float RlRf = Rl*Rf;
        
        float tone_b0 = Y*Wz*RlRf/(RlRf + Y) + Y*Y*Wz/(RlRf+Y); float tone_b1 = RlRf+Y; float tone_b2 = 0.f;
        float tone_a0 = Wp*Wz*Y*RsCs; float tone_a1 = (Wz + Wp + X)*Y*RsCs; float tone_a2 = Y*RsCs;
        float tone_A0 = tone_a0 + tone_a1*c + tone_a2*c_sqrd; // A0 = a0 + a1*c + a2*c^2
        float tone_inv = 1.f / tone_A0;
        float tone_A1 = 2*tone_a0 - 2*tone_a2*c_sqrd;
        float tone_A2 = tone_a0 - tone_a1*c + tone_a2*c_sqrd;
        float tone_B0 = tone_b0 + tone_b1*c + tone_b2*c_sqrd; // A0 = a0 + a1*c + a2*c^2
        float tone_B1 = 2*tone_b0 - 2*tone_b2*c_sqrd;
        float tone_B2 = tone_b0 - tone_b1*c + tone_b2*c_sqrd;
        toneFilt.set_coefficients(tone_B0*tone_inv,tone_B1*tone_inv,tone_B2*tone_inv,1.f,tone_A1*tone_inv,tone_A2*tone_inv);
    }
    
    MyBiquad toneFilt;
    MyBiquad hpf1;
    MyBiquad hpf2;
    MyBiquad op_amp;
    float drive_, tone_, mix_;
    bool bypassed_;
    float c, c_sqrd;
    float Ri = 10000.f; float Rz = 220.f; float Rf = 1000.f; //Rs = Rf = 1k
    float Czs = 0.00000022f; // .22uF same for Cz and Cs
    float Wp = 5000.f;
    float RsCs = 0.00022f; // Rs*Cs
    float two_pi = juce::MathConstants<float>::twoPi;
};

#endif // my_drive_h
