//*****************************************************************************
//
// Configure the device pins for different signals
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//*****************************************************************************
//
// This file was automatically generated on 22/07/2016 at 16:02:59
// by TI PinMux version
//
//*****************************************************************************

#include <ptgmed_inc/ADC_pinout.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"

//*****************************************************************************
//
//! \addtogroup pinout_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Configures the device pins for the customer specific usage.
//!
//! \return None.
//
//*****************************************************************************
void
PinoutSetADC(void)
{
    // Enable Peripheral Clocks
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);

	// Configure the GPIO Pin Mux for PE3
	// for AIN0
	MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
	// Configure the GPIO Pin Mux for PE2
	// for AIN1
	MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);
	// Configure the GPIO Pin Mux for PE1
	// for AIN2
	MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);
	// Configure the GPIO Pin Mux for PE0
	// for AIN3
	MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);
	// Configure the GPIO Pin Mux for PD7
	// for AIN4
	MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_7);
	// Configure the GPIO Pin Mux for PD6
	// for AIN5
	MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_6);
	// Configure the GPIO Pin Mux for PD5
	// for AIN6
	MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_5);
	// Configure the GPIO Pin Mux for PD4
	// for AIN7
	MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_4);
	// Configure the GPIO Pin Mux for PE5
	// for AIN8
	MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_5);
    // Configure the GPIO Pin Mux for PE4
	// for AIN9
	MAP_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);
	// Configure the GPIO Pin Mux for PB4
	// for AIN10
	MAP_GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_4);
	// Configure the GPIO Pin Mux for PB5
	// for AIN11
	MAP_GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_5);
	// Configure the GPIO Pin Mux for PD3
	// for AIN12
	MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3);
	// Configure the GPIO Pin Mux for PD2
	// for AIN13
	MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_2);
	// Configure the GPIO Pin Mux for PD1
	// for AIN14
	MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_1);
	// Configure the GPIO Pin Mux for PD0
	// for AIN15
	MAP_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);
	// Configure the GPIO Pin Mux for PK0
	// for AIN16
	MAP_GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_0);
    // Configure the GPIO Pin Mux for PK1
	// for AIN17
	MAP_GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_1);
	// Unlock the Port Pin and Set the Commit Bit
	HWREG(GPIO_PORTD_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTD_BASE+GPIO_O_CR)   |= GPIO_PIN_7;
	HWREG(GPIO_PORTD_BASE+GPIO_O_LOCK) = 0x0;
	// Configure the GPIO Pin Mux for PK2
	// for AIN18
	MAP_GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_2);
	// Configure the GPIO Pin Mux for PK3
	// for AIN19
	MAP_GPIOPinTypeADC(GPIO_PORTK_BASE, GPIO_PIN_3);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

