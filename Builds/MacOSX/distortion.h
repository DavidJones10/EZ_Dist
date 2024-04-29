//
//  distortion.h
//  EZ_Dist
//
//  Created by David Jones on 4/13/24.
//

#ifndef distortion_h
#define distortion_h

#include <JuceHeader.h>

struct Distortion {
public:
    void init(float sampleRate, int samplesPerBlock);
    float process(float input);
    void set_params(float param_1, float param_2, float param_3, float param_4);
    
private:
    float amount, lpf_fc, hpf_fc, Q;
}

void Distortion::Distortion(){}
void Distortion::Distortion~()


#endif /* distortion_h */
