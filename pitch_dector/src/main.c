/**************************************************************************************************
* @file		Projects\AudioAdjuster\src\main.c
*
* Summary:
*         	main function for the pitch detection project
*
* ToDo:
*     		none
*
* Originator:
*     		Saul Goldblatt
*
* History:
* 			Version 1.00	18/03/2016	Saul Goldblatt	Initial Version copied from Audio adjuster project with modified directories
*      		Version 1.01    18/03/2016	Saul Goldblatt	FFT implemented
*      		Version 1.02    18/03/2016	Saul Goldblatt	split frequency range into thirds and uses this for bases of LED high, med, low 
*      		Version 1.03	18/03/2016	Saul Goldblatt	Uses proper peak detection method
*			Version 1.04	23/03/2016	Saul Goldblatt	replays the peak frequency with s1
*
***************************************************************************************************/
#include <p33FJ256GP506.h>
#include <sask.h>
#include <ex_sask_generic.h>
#include <ex_sask_led.h>
#include <dsp.h>
#include <ADCChannelDrv.h>
#include <OCPWMDrv.h>
#include "..\inc\frequency_processing.h"

#include "..\inc\modulate.h"

#include "..\inc\transform.h"

#define FRAME_SIZE 			128

//Allocate memory for input and output buffers
fractional		adcBuffer		[ADC_CHANNEL_DMA_BUFSIZE] 	__attribute__((space(dma)));
fractional		ocPWMBuffer		[OCPWM_DMA_BUFSIZE]		__attribute__((space(dma)));

//variables for FFT
fractcomplex compx[FRAME_SIZE]__attribute__ ((space(ymemory),far));
fractcomplex compX[FRAME_SIZE]__attribute__ ((space(ymemory),far));
fractional outputSignal[FRAME_SIZE];
double compXfilteredAbs[FRAME_SIZE]__attribute__ ((space(ymemory),far));

//variables for audio processing
fractional		frctAudioIn			[FRAME_SIZE]__attribute__ ((space(xmemory),far));
fractional		frctAudioOut		[FRAME_SIZE]__attribute__ ((space(xmemory),far));

//Instantiate the drivers
ADCChannelHandle adcChannelHandle;
OCPWMHandle 	ocPWMHandle;

//Create the driver handles
ADCChannelHandle *pADCChannelHandle 	= &adcChannelHandle;
OCPWMHandle 	*pOCPWMHandle 		= &ocPWMHandle;

/*
float getFrequency(int samplingRate, int frameSize, int FFTPosition){
	
	float frequency = (samplingRate/2)*(FFTPosition/64);
	return frequency; 
}
*/
float testFrequency = 0;
	

int main(void)
{
	int i;
	int max=0;
	int maxPosition=0;
	float freq;
	ex_sask_init( );

	/*Initialise Audio input and output function*/
	ADCChannelInit	(pADCChannelHandle,adcBuffer);			
	OCPWMInit		(pOCPWMHandle,ocPWMBuffer);			

	/*Start Audio input and output function*/
	ADCChannelStart	(pADCChannelHandle);
	OCPWMStart		(pOCPWMHandle);	
	
	/*start processing loop*/	
	while(1)
	{   			    
		if(SWITCH_S1==0){
		freq= 4000.0 *(maxPosition/64.0);
		createSimpleSignal(freq, FRAME_SIZE, outputSignal);
		RED_LED=0;
			
		}
		else if (SWITCH_S1==1){
			for(i=0;i<FRAME_SIZE;i++){
			outputSignal[i] = 0;
			}
		}
		else if (SWITCH_S2==0){
				freq=2* 4000.0 *(maxPosition/64.0);
			createSimpleSignal(freq, FRAME_SIZE, outputSignal);
			}
		else if (SWITCH_S2==1){
			for(i=0;i<FRAME_SIZE;i++){
				outputSignal[i] = 0;
			}
		}
		/*Wait till the ADC has a new frame available*/
		while(ADCChannelIsBusy(pADCChannelHandle));
		/*Read in the Audio Samples from the ADC*/
		ADCChannelRead	(pADCChannelHandle,frctAudioIn,FRAME_SIZE);
		//work in the frequency domain
		fourierTransform(FRAME_SIZE,compX,frctAudioIn);
		
		
		//take absolute value of the FFT result
	/*
		for(i=1;i<FRAME_SIZE/2;i++){
				compXfilteredAbs[i] = pow(compX[i].real,2) + pow(compX[i].imag,2);	
		}
		*/
		getAbsSqrd(compX,compXfilteredAbs,FRAME_SIZE);	
		
		//find the peak frequency	
		for(i = 0; i<FRAME_SIZE/2;i++){
				if(compXfilteredAbs[i] > max){
				max = compXfilteredAbs[i];
				maxPosition = i;
				}
		}
		//determine which led to display based on peak fequency
		if(max< 100){
			GREEN_LED=1;
			RED_LED = 1;
			YELLOW_LED=1;
		}
		
		else if(maxPosition<FRAME_SIZE/6){
				GREEN_LED=0;
					RED_LED = 1;
					YELLOW_LED=1;
		}
		else if(maxPosition>FRAME_SIZE/6 && maxPosition<FRAME_SIZE/3){
				GREEN_LED=1;
					RED_LED = 1;
					YELLOW_LED=0;
		}
		else if(maxPosition>FRAME_SIZE/3){
				GREEN_LED=1;
					RED_LED = 0;
					YELLOW_LED=1;
		}
		else{
			 GREEN_LED=1;
					RED_LED = 1;
					YELLOW_LED=1;	
		}
		
						
		//Wait till the OC is available for a new frame
		while(OCPWMIsBusy(pOCPWMHandle));	
		//Write the real part of the frequency shifted complex audio signal to the output
		OCPWMWrite (pOCPWMHandle,outputSignal,FRAME_SIZE);
		
	}
}
