#ifndef __SAMPLES_CONFIG_H__
#define __SAMPLES_CONFIG_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include "../includes_protegemd/system.h"

//*****************************************************************************
//
// Defines.
//
//*****************************************************************************
//
//#define CLK_FREQ            	120000000	// Main CPU clock -> 120MHz
//#define SAMPLE_FREQ         	15360		// ADC Sample Frequency: 60Hz * 256 Samples = 15360Hz
//#define CH_SAMPLE_NUMBER		256			// Samples per channel
//#define ADC_SAMPLE_BUF_SIZE 	1024 		// Max uDMA transfer buffer, it sucks->Max Buffer size of uDMA is 1024
//#define OVER_SAMPLE_VALUE	16			// Over sample value -> each sample is the average of 16 samples
//#define ADC_CHANNEL_OFFSET  	2048		    // Offset of the ADC channel -> ADC_Resolution/2 = 4096/2 = 2048
//#define ADC_INPUT_SCALE		3.3/4095	    // Vmax / ADC_Resolution -> 3.3V/(2^12-1)
//#define NUMBER_OF_CH_ADC		8			// Using Sample sequencer FIFO depth 8, so 8 channels per ADC unit.

//TODO: Try to fix this issue, awful fist samples :(
#define IGNORE_INIT_SAMPLES	0			// Leave DMA and ADC initialize, 2 is good enough

//*****************************************************************************
//
// Prototypes.
//
//*****************************************************************************

extern void ADC_init(void);
extern void DMA_init(void);
extern void TIMER_init(void);
extern void ADC_Seq0_ISR();		//Interrupt service of ADC Sequencer 0
extern void ADC_Seq1_ISR();		//Interrupt service of ADC Sequencer 1

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __SAMPLES_CONFIG_H__
