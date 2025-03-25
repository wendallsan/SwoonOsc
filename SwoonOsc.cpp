#include "daisy_seed.h"
#include "daisysp.h"
#include "SupaSawOsc.h"

using namespace daisy;
using namespace daisysp;

#define SAMPLE_RATE 48000.f
#define NUM_SAMPLES 4

#define PIN_VPO seed::A0
#define PIN_FREQ_ADJ seed::A1
#define PIN_DRIFT_ADJ seed::A2
#define PIN_SHIFT_ADJ seed::A3
#define PIN_SUB_ADJ seed::A4
#define PIN_COARSE seed::A5
#define PIN_FINE seed::A6
#define PIN_DRIFT seed::A7
#define PIN_SHIFT seed::A8
#define PIN_SUB seed::A9
#define V_TO_O 261.625565f

enum inputs { 
	IN_VPO, 
	IN_FREQ_ADJ,
	IN_DRIFT_ADJ, 
	IN_SHIFT_ADJ, 
	IN_SUB_ADJ,
	IN_COARSE, 
	IN_FINE, 
	IN_DRIFT, 
	IN_SHIFT, 
	INS_COUNT
};

DaisySeed hw;
SupaSawOsc osc;

void AudioCallback( AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size ){
	for( size_t i = 0; i < size; i++ ){
		out[0][i] = osc.Process();
		out[1][i] = 0.f;
	}
}

void updateValues(){
	osc.SetDrift( fclamp( hw.adc.GetFloat( IN_DRIFT ) +  hw.adc.GetFloat( IN_DRIFT_ADJ ), 0.f, 1.f ) );
	osc.SetShift( fclamp( hw.adc.GetFloat( IN_SHIFT ) +  hw.adc.GetFloat( IN_SHIFT_ADJ ), 0.f, 1.f ) );	
	// vpo logic taken from:
	// https://forum.electro-smith.com/t/frequency-to-1v-oct-and-a-few-other-conversions/4592/2
	float vpo = hw.adc.GetFloat( IN_VPO ) * 5.f;
	float vpoFreq = pow( 2, vpo ) * V_TO_O;

	osc.SetFreq( 
		fclamp( 
			fmap( 
				fclamp( 
					hw.adc.GetFloat( IN_COARSE ) + ( hw.adc.GetFloat( IN_FINE ) / 100.f ) + hw.adc.GetFloat( IN_FREQ_ADJ ), 
					0.f, 
					1.f 
				), 
				20.f, 
				20000.f 
			) + vpoFreq, 
			20.f, 
			2000.f 
		) 
	);
}

void initAdc(){
	AdcChannelConfig adc_cfg[ INS_COUNT ];
    adc_cfg[ IN_COARSE ].InitSingle( PIN_COARSE );
    adc_cfg[ IN_FINE ].InitSingle( PIN_FINE );
    adc_cfg[ IN_DRIFT ].InitSingle( PIN_DRIFT );
    adc_cfg[ IN_SHIFT ].InitSingle( PIN_SHIFT );
    adc_cfg[ IN_FREQ_ADJ ].InitSingle( PIN_FREQ_ADJ );
    adc_cfg[ IN_DRIFT_ADJ].InitSingle( PIN_DRIFT_ADJ );
    adc_cfg[ IN_SHIFT_ADJ ].InitSingle( PIN_SHIFT_ADJ);
    adc_cfg[ IN_FREQ_ADJ ].InitSingle( PIN_FREQ_ADJ );
    adc_cfg[ IN_DRIFT_ADJ ].InitSingle( PIN_FREQ_ADJ );
    adc_cfg[ IN_SHIFT_ADJ ].InitSingle( PIN_SHIFT_ADJ );
    adc_cfg[ IN_VPO ].InitSingle( PIN_VPO );
	hw.adc.Init( adc_cfg, INS_COUNT );
	hw.adc.Start();	
}

int main( void ){
	hw.Init();
	hw.SetAudioBlockSize( NUM_SAMPLES );
	hw.SetAudioSampleRate( SaiHandle::Config::SampleRate::SAI_48KHZ );
	osc.Init( SAMPLE_RATE );
	initAdc();
	updateValues();
	hw.StartAudio( AudioCallback );
	while( true ) updateValues();
}
