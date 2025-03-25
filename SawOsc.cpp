#include "Utility/dsp.h"
#include "SawOsc.h"

using namespace daisysp;

float SawOsc::Process(){
        float out = -1.f * ( ( phase_ * 2.f ) - 1.f );
        phase_ += phase_inc_;
        if( phase_ > 1.f ) phase_ -= 1.0f;
        return out;
}
float SawOsc::CalcPhaseInc( float f ){ return f * sr_recip_; }