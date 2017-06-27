/*
 * system.h
 *
 *  Created on: 18 de ago de 2016
 *      Author: skelter
 */

#ifndef INCLUDES_PROTEGEMD_SYSTEM_H_
#define INCLUDES_PROTEGEMD_SYSTEM_H_

/* Default parameters */
#define CLK_FREQ                120000000   // Main CPU clock -> 120MHz
#define SAMPLE_FREQ             15360       // ADC Sample Frequency: 60Hz * 256 Samples = 15360Hz
#define CH_SAMPLE_NUMBER        256         // Samples per channel
#define ADC_SAMPLE_BUF_SIZE     1024        // Max uDMA transfer buffer, it sucks->Max Buffer size of uDMA is 1024
#define OVER_SAMPLE_VALUE       16          // Over sample value -> each sample is the average of 16 samples
#define ADC_CHANNEL_OFFSET      2048        // Offset of the ADC channel -> ADC_Resolution/2 = 4096/2 = 2048
#define ADC_INPUT_SCALE         3.3/4095    // Vmax / ADC_Resolution -> 3.3V/(2^12-1)
#define NUMBER_OF_CH_ADC        8           // Using Sample sequencer FIFO depth 8, so 8 channels per ADC unit.

//#define NumbersOfCycles         3
//#define RMS_Limit               0.3

/* Standard variables definitions */
#include <stdint.h>
#include <stdbool.h>

/* CMSIS DSP library */
#include "arm_math.h"   //Use CMSIS ARM library for fast math operation.

/* Sturcts */
/*
 * Electrical outlet struct.
 *
 */
typedef struct
{
    char id[8];
    float32_t dif_samples[CH_SAMPLE_NUMBER];
    float32_t ph_samples[CH_SAMPLE_NUMBER];
    float32_t dif_rms;
    float32_t ph_rms;
    float32_t voltage;
    float32_t fft[CH_SAMPLE_NUMBER / 2];
    uint64_t events;
} outlet;

/*
 * Electrical Panel (gas panel)
 *
 */
typedef struct
{
    float32_t voltage_samples_L1[CH_SAMPLE_NUMBER];
    float32_t voltage_samples_L2[CH_SAMPLE_NUMBER];
    float32_t voltage_samples_L3[CH_SAMPLE_NUMBER];
    float32_t L1_rms;
    float32_t L2_rms;
    float32_t L3_rms;

} panel_voltages;

enum connection
{
    outlet1_diff = 0, outlet1_phase = 1, outlet1_4_voltage_L1 = 13
};

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>		//needed for any Log_info() call
#include <xdc/cfg/global.h>
#include <xdc/runtime/Error.h> 		// for Error Block

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

/* NDK BSD support */
#include <sys/socket.h>

/* Tivaware Header files */
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_adc.h"
#include "inc/hw_gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/udma.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"


/* Board Header file */
#include "../includes_protegemd/Samples_config.h"
#include "../includes_protegemd/Network_config.h"
#include "../includes_protegemd/Board.h"
#include "../includes_protegemd/ADC_pinout.h"


#endif /* INCLUDES_PROTEGEMD_SYSTEM_H_ */
