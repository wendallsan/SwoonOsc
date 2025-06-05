# Swoon Oscillator

## Author

Song Naga

## Description

A Monophonic SupaSaw Oscillator for Eurorack systems modelled from the famous Roland JP-8000 preset.  

This is a monopohonic implementation of the oscillators used in the **Takshaka Desktop Synthesizer** for Eurorack systems.  It consists of a stack of 7 sawtooth oscillators that are detuned and mixed together in varying amounts according to the module's DRIFT and SHIFT controls.  

It features manual controls for Frequency (with Coarse and Fine adjustment knobs), Drift, and Shift, as well as adjustable CV inputs for these controls, a dedicated Volts-Per-Octave  input, reset trigger input, and a single output.  


DRIFT and SHIFT knobs adjust supersaw detune and volume levels of the oscillator stack that produces the SupaSaw effect.  TRIG input randomly resets the phase of each oscillator in the stack when it receives a gate or trigger signal. 

The SupaSaw Oscillator used in Swoon is based on the thesis ["How to Emulate the Super Saw"](https://adamszabo.com/internet/adam_szabo_how_to_emulate_the_super_saw.pdf) written by Adam Szabo in 2010.
