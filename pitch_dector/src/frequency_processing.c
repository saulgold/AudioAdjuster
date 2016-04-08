#include "..\inc\frequency_processing.h"

void getAbsSqrd(fractcomplex *inputSignal, double *absOutputSignal, int frameSize)
{
		int i;
		for(i=1;i<frameSize/2;i++){
				absOutputSignal[i] = pow(inputSignal[i].real,2) + pow(inputSignal[i].imag,2);	
		}	
		
}

void getFrequency(int position,double frequency, int frameSize)
{
	frequency = 8000*(position/128);
}
void findPeakFrequency(double *inputSignal, int maxFreqPosition,int frameSize)
{
	int i;
	int max =0;
	for(i = 0; i<frameSize/2;i++){
		if(inputSignal[i] > max){
			max = inputSignal[i];
			maxFreqPosition = i;
			}
	}	
}
