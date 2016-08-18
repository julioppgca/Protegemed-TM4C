/*
 * system.h
 *
 *  Created on: 18 de ago de 2016
 *      Author: skelter
 */

#ifndef PTGMED_INC_SYSTEM_H_
#define PTGMED_INC_SYSTEM_H_

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

/* NDK BSD support */
#include <sys/socket.h>

/* Board Header file */
#include <ptgmed_inc/Samples_config.h>
#include <ptgmed_inc/Network_config.h>
#include <ptgmed_inc/Board.h>
#include <ptgmed_inc/ADC_pinout.h>

/* Standard variables definitions */
#include <stdint.h>
#include <stdbool.h>

/* Tivaware Header files */
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

/* CMSIS DSP library */
#include "arm_math.h"	//Use CMSIS ARM library for fast math operation.


#endif /* PTGMED_INC_SYSTEM_H_ */
