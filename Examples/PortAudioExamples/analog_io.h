// Header file for analog_io.c functions, for simplified PortAudio API
// Copyright (c) 2024 Delta Numerix All rights reserved.

int analog_open(int SampleRate, void (*analog_isr)(void));
int analog_close(void);

volatile short adc_in0, adc_in1, dac_out0, dac_out1;
