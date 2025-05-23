#include "daisy_seed.h"
#include "SupaSawOsc.h"
#include "daisysp.h"
#include <algorithm>
namespace daisysp {
    void SupaSawOsc::Init( float sampleRate ){
        sampleRate_ = sampleRate;
        freq_ = 220.0; // INIT FREQ TO 220HZ
        for( int i = 0; i < NUMBER_OF_SAWS; i++ ){
            superSaws_[ i ].Init( sampleRate_ );
            /*
            RANDOMIZE PHASE AND SAVE FOR LATER USAGE-- 
            WE'LL EVENTUALLY WANT TO CHANGE PHASE WITH EACH NEW NOTE USING PhaseAdd()
            AND IT HELPS TO KNOW THE CURRENT PHASE TO DO THAT
            */
            phases_[ i ] = rand() / RAND_MAX;
            superSaws_[ i ].PhaseAdd( phases_[ i ] );
        }
        hpf_.Init( sampleRate_ );
        float hpfFreq = freq_;
        hpf_.SetFreq( hpfFreq );
    }   
    float SupaSawOsc::Process(){        
		float superSawSignal = 0.0;
		for( int i = 0; i < NUMBER_OF_SAWS; i++ ) 
            superSawSignal += ( superSaws_[ i ].Process() );
        return hpf_.Process( superSawSignal );        
    }
    void SupaSawOsc::SetFreq( float freq ){
        freq_ = freq;
        // WE DON'T NEED TO SET THE SET FREQ OF THE OSCILLATORS, THEY WILL 
        // CHANGE FREQUENCY NEXT TIME SETDRIFT IS CALLED
    }
    void SupaSawOsc::Reset(){
        for( int i = 0; i < NUMBER_OF_SAWS; i++ ){
            float rando = rand() / RAND_MAX;
            float newPhase = rando + phases_[ i ],
                delta;
            /*
            PhaseAdd CAN'T SET THE PHASE HIGHER THAN 1.0, SO WE NEED TO KEEP TRACK
            OF IT AND IF OUR NEW PHASE IS GREATER THAN 1, WE NEED TO HANDLE IT
            */
            if( newPhase > 1.0 ) {
                delta = 1.0 - rando;
                newPhase = newPhase - 1.0;   
            } else delta = rando;
            phases_[ i ] = newPhase;
            superSaws_[ i ].PhaseAdd( delta );
        }
    }
    void SupaSawOsc::SetDrift( float drift ){
        drift_ = drift;
        intensityKnobCurveIndex_ = drift * CURVE_POINTS;
        for( int i = 0; i < NUMBER_OF_SAWS; i++ ){ // ADJUST AMPLITUDES
			superSaws_[ i ].SetAmp( 
				i == 3?
					1.0 - fmap( drift, 0.0, ampAmounts_[ i ] ) :
					lookupAmp( drift ) * ampAmounts_[ i ]
			);
		}
    }
    void SupaSawOsc::SetShift( float shift ){
        shift_ = shift;
        detuneKnobCurveIndex_ = shift * CURVE_POINTS;
        for( int i = 0; i < NUMBER_OF_SAWS; i++ ){ // ADJUST FREQUENCIES
			superSaws_[ i ].SetFreq( 
				freq_ + 
				lookupDetune( shift ) * detunes_[ i ] * freq_
			);
		}
    }
    float SupaSawOsc::lookupDetune( float knobAmount ){
        float minDetune = detuneLookupTable_[ detuneKnobCurveIndex_ ];
        float maxDetune = detuneLookupTable_[ detuneKnobCurveIndex_ + 1 ];
        float minCurveValue = ( float )detuneKnobCurveIndex_ / CURVE_POINTS;
        float maxCurveValue = ( ( float )detuneKnobCurveIndex_ + 1 ) / CURVE_POINTS;
        return minDetune + ( ( maxDetune - minDetune ) * ( ( knobAmount - minCurveValue ) / ( maxCurveValue / minCurveValue ) ) );
    }
    float SupaSawOsc::lookupAmp( float knobAmount ){
        float minAmp = ampLookupTable_[ intensityKnobCurveIndex_  ];
        float maxAmp = ampLookupTable_[ intensityKnobCurveIndex_ + 1];
        float minCurveValue = ( float )intensityKnobCurveIndex_ / CURVE_POINTS;
        float maxCurveValue = ( ( float )intensityKnobCurveIndex_ + 1 ) / CURVE_POINTS;
        return minAmp + ( ( maxAmp - minAmp ) * ( ( knobAmount - minCurveValue ) / ( maxCurveValue / minCurveValue ) ) );	
    }
}