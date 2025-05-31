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
#define V_TO_O 261.625565f
enum inputs { IN_VPO, IN_FREQ_ADJ, IN_DRIFT_ADJ, IN_SHIFT_ADJ, IN_COARSE, IN_FINE, IN_DRIFT, IN_SHIFT, INS_COUNT };
DaisySeed hw;
SupaSawOsc osc;
Switch resetTrig;
void AudioCallback( AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size ){ for( size_t i = 0; i < size; i++ ) out[0][i] = osc.Process(); }
void updateValues(){
	osc.SetDrift( fclamp( hw.adc.GetFloat( IN_DRIFT ) + hw.adc.GetFloat( IN_DRIFT_ADJ ), 0.f, 0.99f ) );
	osc.SetShift( fclamp( hw.adc.GetFloat( IN_SHIFT ) + hw.adc.GetFloat( IN_SHIFT_ADJ ), 0.f, 0.99f ) );
	float coarseMidiValue = fmap( hw.adc.GetFloat( IN_COARSE ), 0.f, 60.f ), fineDelta = coarseMidiValue / 24.f;
	osc.SetFreq( 
		mtof( 
			fclamp( 
				coarseMidiValue + // COARSE VALUE
				fmap( hw.adc.GetFloat( IN_FINE ), -fineDelta, fineDelta ) + // FINE VALUE
				fmap( hw.adc.GetFloat( IN_VPO ), 0.f, 60.f ), // VPO VALUE
				0.f, 
				127.f
			) 
		) 
	);
    resetTrig.Debounce();
	if( resetTrig.RisingEdge() ) osc.Reset();
}
void initAdc(){
	AdcChannelConfig adc_cfg[ INS_COUNT ];
	adc_cfg[ IN_VPO ].InitSingle( PIN_VPO );
	adc_cfg[ IN_FREQ_ADJ ].InitSingle( PIN_FREQ_ADJ );
	adc_cfg[ IN_DRIFT_ADJ].InitSingle( PIN_DRIFT_ADJ );
	adc_cfg[ IN_SHIFT_ADJ ].InitSingle( PIN_SHIFT_ADJ);
	adc_cfg[ IN_COARSE ].InitSingle( PIN_COARSE );
	adc_cfg[ IN_FINE ].InitSingle( PIN_FINE );
	adc_cfg[ IN_DRIFT ].InitSingle( PIN_DRIFT );
	adc_cfg[ IN_SHIFT ].InitSingle( PIN_SHIFT );
	hw.adc.Init( adc_cfg, INS_COUNT );
	hw.adc.Start();	
}
int main( void ){
	hw.Init();
	hw.SetAudioBlockSize( NUM_SAMPLES );
	hw.SetAudioSampleRate( SaiHandle::Config::SampleRate::SAI_48KHZ );
	initAdc();
    resetTrig.Init( hw.GetPin( 14 ), 1000 );
	updateValues();
	osc.Init( SAMPLE_RATE );
	hw.StartAudio( AudioCallback );
	while( true ) updateValues();
}