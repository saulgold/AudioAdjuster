#ifndef __FREQUENCY_PROCESSING_H__
#define __FREQUENCY_PROCESSING_H__
#include <dsp.h>

void getAbsSqrd(fractcomplex *inputSignal, double *absOutputSignal, int frameSize);
void getFrequency(int position,double frequency, int frameSize);
void findPeakFrequency(double *inputSignal, int maxFreqPosition, int frameSize);
#endif
