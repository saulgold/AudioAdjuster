#include <frequency_processing.h>

float getFrequency(int samplingRate, int frameSize, int FFTPosition){
	
	float frequency = (samplingRate/2)*(FFTPosition/64);
	return frequency; 
}
