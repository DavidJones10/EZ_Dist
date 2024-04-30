/*
  ==============================================================================

    my_biquad.h
    Created: 29 Apr 2024 9:11:44pm
    Author:  David Jones

  ==============================================================================
*/

#pragma once

struct MyBiquad{
public:
    inline void init(float sample_rate);
    inline float process(float in_sample);
    inline void set_coefficients(float b0, float b1, float b2, float a0, float a1, float a2);
    
private:
    float b0_, b1_, b2_, a0_, a1_, a2_;
    float sample_rate_;
    float x1, x2, y1, y2;
};

void MyBiquad::init(float sample_rate){
    sample_rate_ =  sample_rate;
}

float MyBiquad::process(float in_sample){
    float output = b0_ * in_sample + b1_ * x1 + b2_ * x2 - a1_ * y1 - a2_ * y2;
    x2 = x1;
    x1 = in_sample;
    y2 = y1;
    y1 = output;
    return output/a0_;
}

void MyBiquad::set_coefficients(float b0, float b1, float b2, float a0, float a1, float a2){
    b0_ = b0;
    b1_ = b1;
    b2_ = b2;
    a0_ = a0;
    a1_ = a1;
    a2_ = a2;
}
