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

//#define __DEBUG_OVERRIDE_INPUT
//#define __DEBUG_FILTERS
//#define __DEBUG_SHIFTERS
//#define __DEBUG_TRANSFORMS

#define FRAME_SIZE 			128
#define UPPER_CARRIER_FREQ 		625	
#define LOWER_CARRIER_FREQ 		62.5
#define CARRIER_INC				62.5
#define CARRIER_DEC				62.5
#define PRESSED					1
#define UNPRESSED				0

//Modes are used to change the way the device does things, pressing switch 1 changes the mode
#define MODE_DO_NOTHING			0 //the device passes the audio straight through to the output
#define MODE_BAND_PASS_FILTER	1 //the device uses the band pass filter to remove negative audio frequencies
#define MODE_BAND_PASS_SHIFT	3 //the device band pass filters and shifts the audio frequencies
#define MODE_LOW_PASS_FILTER	2 //the device uses the shifted low pass filter to remove negative audio frequencies
#define MODE_LOW_PASS_SHIFT		4 //the device uses shifted low pass filters and shifts the audio frequencies
#define MODE_FREQ_DOMAIN		5 //the device works on the audio signal in the frequency domain
#define MODE_TOTAL				6

//Allocate memory for input and output buffers
fractional		adcBuffer		[ADC_CHANNEL_DMA_BUFSIZE] 	__attribute__((space(dma)));
fractional		ocPWMBuffer		[OCPWM_DMA_BUFSIZE]		__attribute__((space(dma)));

//variables for FFT
fractcomplex compx[FRAME_SIZE]__attribute__ ((space(ymemory),far));
fractcomplex compX[FRAME_SIZE]__attribute__ ((space(ymemory),far));
fractcomplex compXfiltered[FRAME_SIZE]__attribute__ ((space(ymemory),far));
fractcomplex compXshifted[FRAME_SIZE]__attribute__ ((space(ymemory),far));
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

//	float fCarrierFrequency = 1;
//	createComplexSignal(fCarrierFrequency,FRAME_SIZE,compCarrierSignal);
//	
//	#ifdef __DEBUG_OVERRIDE_INPUT
//		float debugFrequency;
//	#endif
//
//	#ifdef __DEBUG_FILTERS//if in filter debug mode create a test signal
//		debugFrequency = 0;
//		createSimpleSignal(debugFrequency,FRAME_SIZE,frctAudioIn);
//	#endif
//
//	#ifdef __DEBUG_SHIFTERS//if in shifter debug mode create a constant test signal
//		debugFrequency = 1250;
//		createSimpleSignal(debugFrequency,FRAME_SIZE,frctAudioIn);		
//	#endif
//
//	#ifdef __DEBUG_TRANSFORMS//if in transform debug mode create a constant test signal
//		debugFrequency = 1250;
//		createSimpleSignal(debugFrequency,FRAME_SIZE,frctAudioIn);		
//	#endif
	
//	initFilter();

	ex_sask_init( );

	//Initialise Audio input and output function
	ADCChannelInit	(pADCChannelHandle,adcBuffer);			
	OCPWMInit		(pOCPWMHandle,ocPWMBuffer);			

	//Start Audio input and output function
	ADCChannelStart	(pADCChannelHandle);
	OCPWMStart		(pOCPWMHandle);	
		double sumLF;
		double sumMF;
		double sumHF;
		double sumTOT;
		int max, maxPosition;
	while(1)
	{	max=0;
		maxPosition=0;
		sumLF=0;
		sumMF=0;
		sumHF=0;
		sumTOT = 0;	
//	YELLOW_LED=0;
		
	//	#ifndef __DEBUG_OVERRIDE_INPUT//if not in debug mode, read audio in from the ADC
			//Wait till the ADC has a new frame available
			while(ADCChannelIsBusy(pADCChannelHandle));
			//Read in the Audio Samples from the ADC
			ADCChannelRead	(pADCChannelHandle,frctAudioIn,FRAME_SIZE);
	//	#endif

	
				//work in the frequency domain
				fourierTransform(FRAME_SIZE,compX,frctAudioIn);
				//filterNegativeFreq(FRAME_SIZE,compXfiltered,compX);
//				
//			for(i=1;i<FRAME_SIZE/6;i++){
//				sumLF+=(pow(compXfiltered[i].real,2)+pow(compX[i].imag,2)); //calculating abs value not sqrt as dont need to
//				
//			}
//				for(i=FRAME_SIZE/6;i<FRAME_SIZE/3;i++){
//				sumMF += (pow(compXfiltered[i].real,2)+pow(compX[i].imag,2));
//			}
//				for(i=FRAME_SIZE/3;i<FRAME_SIZE/2;i++){
//				sumHF += (pow(compXfiltered[i].real,2)+pow(compX[i].imag,2));
//			}
//			sumTOT = sumLF + sumMF +sumHF;
//				
//		//	if(sumLF>5&&sumMF>5&&sumHF<5
//			if((sumLF/sumTOT)>(sumMF/sumTOT)&&(sumLF/sumTOT)>(sumHF/sumTOT)){
//					GREEN_LED=0;
//					RED_LED = 1;
//					YELLOW_LED=1;
//			}
//			else if((sumMF/sumTOT)>(sumLF/sumTOT)&&(sumMF/sumTOT)>(sumHF/sumTOT)){
//					GREEN_LED=1;
//					RED_LED = 1;
//					YELLOW_LED=0;
//			}
//			else if((sumHF/sumTOT)>(sumLF/sumTOT)&&(sumHF/sumTOT)>(sumMF/sumTOT)){
//					GREEN_LED=1;
//					RED_LED = 0;
//					YELLOW_LED=1;
//			}
//			else{
//			        GREEN_LED=1;
//					RED_LED = 1;
//					YELLOW_LED=1;
//			}


		for(i=1;i<FRAME_SIZE/2;i++){
				compXfilteredAbs[i] = pow(compX[i].real,2) + pow(compX[i].imag,2);	
		}	
			
		for(i = 0; i<FRAME_SIZE/2;i++){
				if(compXfilteredAbs[i] > max){
				max = compXfilteredAbs[i];
				maxPosition = i;
				}
		}
		
		if(maxPosition<FRAME_SIZE/6){
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
		
		
		
//			for(i=0;i<128;i++){
//			
//				if(compX[i].real>10 ){
//					GREEN_LED=0;
//					RED_LED = 1;
//					YELLOW_LED=1;
//				}
//					else if(compX[i].real>20 ){
//					GREEN_LED=0;
//					RED_LED = 1;
//					YELLOW_LED=0;
//				}
//				else{
//					GREEN_LED=1;
//					RED_LED = 1;
//					YELLOW_LED=1;
//				}
//			}
			//	shiftFreqSpectrum(FRAME_SIZE,iShiftAmount,compXshifted,compXfiltered);
			//	inverseFourierTransform(FRAME_SIZE,frctAudioOut,compXshifted);
					
				//Wait till the OC is available for a new frame
		while(OCPWMIsBusy(pOCPWMHandle));	
		//Write the real part of the frequency shifted complex audio signal to the output
		OCPWMWrite (pOCPWMHandle,frctAudioOut,FRAME_SIZE);
		
	}
}
