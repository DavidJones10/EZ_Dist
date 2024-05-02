#pragma once
/*
  ==============================================================================

    my_biquad.h
    Created: 29 Apr 2024 9:11:44pm
    Author:  David Jones

  ==============================================================================
*/

#ifndef my_biquad_h
#define my_biquad_h

struct MyBiquad {
public:
    void init() {
        v1 = v2 = 0.0f;

        b0_ = 1.0f;
        b1_ = b2_ = 0.0f;
        a1_ = a2_ = 0.0f;
    }

    float process(float in_sample) {
        float v0 = in_sample - a1_ * v1 - a2_ * v2;

        float output = b0_ * v0 + b1_ * v1 + b2_ * v2;

        v2 = v1;
        v1 = v0;

        return output;
    }

    void set_coefficients(float b0, float b1, float b2, float a0, float a1, float a2) {
        b0_ = b0;
        b1_ = b1;
        b2_ = b2;
        a1_ = a1;
        a2_ = a2;
    }

private:
    float b0_, b1_, b2_, a1_, a2_;
    float v1, v2;
};

struct EZ_LPF{
public:
    void init(float sampleRate){
        sample_rate = sampleRate;
        Q = 1.f;
        fc = 10000.f;
        filt.init();
        calculate_coefficients();
    }
    float process(float in_sample){
        return filt.process(in_sample);
    }
    void set_params(float cutoff, float Q_){
        fc = cutoff;
        Q = Q_;
        calculate_coefficients();
    }
private:
    void calculate_coefficients(){
        float n = 1.f/std::tan(juce::MathConstants<float>::pi * fc / sample_rate);
        const float nSquared = n * n;
        const float invQ = 1.f / Q;
        const float c1 = 1.f / (1 + invQ * n + nSquared);
        filt.set_coefficients(c1, c1 * 2.f, c1,
                              1.f, c1 * 2.f * (1.f - nSquared),
                              c1 * (1 - invQ * n + nSquared));
    }
    float Q, fc;
    float sample_rate;
    MyBiquad filt;
};

#endif // "my_biquad.h"
