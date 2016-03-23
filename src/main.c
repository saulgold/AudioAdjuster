/**************************************************************************************************
* @file		Projects\AudioAdjuster\src\main.c
*
* Summary:
*         	main function for the audio adjuster project
*
* ToDo:
*     		none
*
* Originator:
*     		Andy Watt
*
* History:
* 			Version 1.00	20/04/2013	Andy Watt	Initial Version copied from Microchip Demo Project and modified 
*      		Version 1.01    28/04/2013	Andy Watt	Added filter and modulate function calls 
*      		Version 1.02    01/05/2013	Andy Watt	Added mode switching and low pass filter function calls 
*      		Version 1.03	07/05/2013	Andy Watt	Added transform function calls
*
***************************************************************************************************/
#include <p33FJ256GP506.h>
#include <sask.h>
#include <ex_sask_generic.h>
#include <ex_sask_led.h>
#include <dsp.h>
#include <ADCChannelDrv.h>
#include <OCPWMDrv.h>


#include "..\inc\filter.h"
#include "..\inc\modulate.h"
#include "..\inc\complexmultiply.h"
#include "..\inc\transform.h"

#define FRAME_SIZE 			128

//Allocate memory for input and output buffers
fractional		adcBuffer		[ADC_CHANNEL_DMA_BUFSIZE] 	__attribute__((space(dma)));
fractional		ocPWMBuffer		[OCPWM_DMA_BUFSIZE]		__attribute__((space(dma)));

//variables for FFT
fractcomplex compx[FRAME_SIZE]__attribute__ ((space(ymemory),far));
fractcomplex compX[FRAME_SIZE]__attribute__ ((space(ymemory),far));
fractcomplex compXfiltered[FRAME_SIZE]__attribute__ ((space(ymemory),far));
fractcomplex compXshifted[FRAME_SIZE]__attribute__ ((space(ymemory),far));
fractional testSignal[FRAME_SIZE];

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



int main(void)
{
	int i;


	ex_sask_init( );

	//Initialise Audio input and output function
	ADCChannelInit	(pADCChannelHandle,adcBuffer);			
	OCPWMInit		(pOCPWMHandle,ocPWMBuffer);			

	//Start Audio input and output function
	ADCChannelStart	(pADCChannelHandle);
	OCPWMStart		(pOCPWMHandle);	
	
		int max, maxPosition;
	while(1)
	{   
		max = 0;
		maxPosition =0;
		if(SWITCH_S1==0){
		
			createSimpleSignal(3000, FRAME_SIZE, testSignal);
			RED_LED=0;
			
		}
		else if (SWITCH_S1==1){
			RED_LED=1;
			for(i=0;i<FRAME_SIZE;i++){
				testSignal[i] = 0;
			}
		}
		
		//Wait till the ADC has a new frame available
		while(ADCChannelIsBusy(pADCChannelHandle));
		//Read in the Audio Samples from the ADC
		ADCChannelRead	(pADCChannelHandle,frctAudioIn,FRAME_SIZE);
		//work in the frequency domain
		fourierTransform(FRAME_SIZE,compX,frctAudioIn);
		//filterNegativeFreq(FRAME_SIZE,compXfiltered,compX);
		
		//take absolute value of the FFT result
		for(i=1;i<FRAME_SIZE/2;i++){
				compXfilteredAbs[i] = pow(compX[i].real,2) + pow(compX[i].imag,2);	
		}	
		
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
		
		
		
		//	shiftFreqSpectrum(FRAME_SIZE,iShiftAmount,compXshifted,compXfiltered);
		inverseFourierTransform(FRAME_SIZE,frctAudioOut,compXshifted);
					
		//Wait till the OC is available for a new frame
		while(OCPWMIsBusy(pOCPWMHandle));	
		//Write the real part of the frequency shifted complex audio signal to the output
		OCPWMWrite (pOCPWMHandle,testSignal,FRAME_SIZE);
		
	}
}
