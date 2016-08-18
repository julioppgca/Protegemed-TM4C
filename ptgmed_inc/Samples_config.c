#include <xdc/cfg/global.h> //to get access to statically declared variables in RTOS_protegemed.cfg
#include <ti/drivers/GPIO.h>

#include <ptgmed_inc/ADC_pinout.h>
#include <ptgmed_inc/Samples_config.h>
#include <ptgmed_inc/Board.h>

#include <stdint.h>
#include <stdbool.h>

/* Tivaware Header files */
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_adc.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/udma.h"

extern uint32_t udmaCtrlTable[1024]__attribute__((aligned(1024))); // uDMA control table variable
extern int16_t data_array1[ADC_SAMPLE_BUF_SIZE]={}; // Init to zero in all positions
extern int16_t data_array2[ADC_SAMPLE_BUF_SIZE]={};
extern int16_t data_array3[ADC_SAMPLE_BUF_SIZE]={};
extern int16_t data_array4[ADC_SAMPLE_BUF_SIZE]={};
extern uint32_t uDMATransferCount;
extern uint32_t uDMATransferCount2;

void TIMER_init(void)
{
	// Use Timer for both ADC0 and ADC1

     uint32_t ui32ClockFreq;

    // Set clock frequency
    ui32ClockFreq = SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
    SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480, CLK_FREQ);

    // Enable timer peripheral clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_TIMER0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

    // Configure the timer
    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC_UP);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (ui32ClockFreq / SAMPLE_FREQ)); //

    // Enable timer to trigger ADC
    TimerControlTrigger(TIMER0_BASE, TIMER_A, true);

    // Enable event to trigger ADC
    TimerADCEventSet(TIMER0_BASE, TIMER_ADC_TIMEOUT_A);

    // Enable timer
    TimerEnable(TIMER0_BASE, TIMER_A);
}

void ADC_init(void)
{

	/* ----------- ADC0 Initialization-----------*/

    // Enable ADC peripheral clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_ADC0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));

    // Configure the ADC to use PLL at 480 MHz divided by 15 to get an ADC
    // clock of 32 MHz, 2MSPS.
    ADCClockConfigSet(ADC0_BASE,  ADC_CLOCK_SRC_PLL | ADC_CLOCK_RATE_FULL,  15);
    //ADCClockConfigSet(ADC0_BASE,  ADC_CLOCK_SRC_PLL | ADC_CLOCK_RATE_FULL, 4 );

    // Use Hardware Oversample
    ADCHardwareOversampleConfigure(ADC0_BASE, OVER_SAMPLE_VALUE);

    // Set ADC pinout, need external file -> #include "ADC_pinout.h"
    PinoutSetADC();

    // Disable before configuring
    ADCSequenceDisable(ADC0_BASE, 0);

    // Configure ADC0 sequencer 0:
    // Triggered by timer, highest priority (0)
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_TIMER, 0);

    // Configure ADC0 sequencer 0 step 0:
    // Channel 8, last step insequence, causes interrupt when step is complete
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH2);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH3);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH4);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH5);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH6);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH7
    						| ADC_CTL_END | ADC_CTL_IE);

    // Analog reference is internal
    ADCReferenceSet(ADC0_BASE, ADC_REF_INT);

    //  Enable ADC interrupt
    ADCIntEnableEx(ADC0_BASE, ADC_INT_DMA_SS0);
    IntEnable(INT_ADC0SS0);

    // Enable ADC to use uDMA
    ADCSequenceDMAEnable(ADC0_BASE, 0);

    // Enable ADC
    ADCSequenceEnable(ADC0_BASE, 0);


    /* ----------- ADC1 Initialization-----------*/

    // Enable ADC 1 peripheral clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    SysCtlPeripheralReset(SYSCTL_PERIPH_ADC1);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1));

    //ADCClockConfigSet(ADC0_BASE,  ADC_CLOCK_SRC_PLL | ADC_CLOCK_RATE_FULL, 4 );

    // Use Hardware Oversample
    ADCHardwareOversampleConfigure(ADC1_BASE, OVER_SAMPLE_VALUE);

    // Set ADC pinout, need external file -> #include "ADC_pinout.h"
    //PinoutSetADC();

    // Disable before configuring
    ADCSequenceDisable(ADC1_BASE, 0);

    // Configure ADC0 sequencer 0:
    // Triggered by timer, highest priority (0)
    ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_TIMER, 0);

    // Configure ADC0 sequencer 0 step 0:
    // Channel 8, last step insequence, causes interrupt when step is complete
    ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_CH8);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 1, ADC_CTL_CH9);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 2, ADC_CTL_CH10);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 3, ADC_CTL_CH11);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 4, ADC_CTL_CH12);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 5, ADC_CTL_CH13);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 6, ADC_CTL_CH14);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 7, ADC_CTL_CH15
    						| ADC_CTL_END | ADC_CTL_IE);

    // Analog reference is internal
    ADCReferenceSet(ADC1_BASE, ADC_REF_INT);

    //  Enable ADC interrupt
    ADCIntEnableEx(ADC1_BASE, ADC_INT_DMA_SS0);
    IntEnable(INT_ADC1SS0);

    // Enable ADC to use uDMA
    ADCSequenceDMAEnable(ADC1_BASE, 0);

    // Enable ADC
    ADCSequenceEnable(ADC1_BASE, 0);
}

void DMA_init(void)
{

    /* Enable uDMA clock */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UDMA));

    /* Enable uDMA */
    uDMAEnable();

    // Use channel 24 for ADC1, must replace all 'UDMA_CHANNEL_ADC0' by 'UDMA_CH24_ADC1_0'
    uDMAChannelAssign(UDMA_CH24_ADC1_0);

    /* Set the control table for uDMA */
    uDMAControlBaseSet(udmaCtrlTable);

    /* Disable unneeded attributes of the uDMA channels */
    uDMAChannelAttributeDisable(UDMA_CHANNEL_ADC0, UDMA_ATTR_ALL);

    /* Disable unneeded attributes of the uDMA channels -- ADC1 */
    uDMAChannelAttributeDisable(UDMA_CH24_ADC1_0, UDMA_ATTR_ALL);

    // Only allow burst transfers
	uDMAChannelAttributeEnable(UDMA_CHANNEL_ADC0, UDMA_ATTR_USEBURST);

	// Only allow burst transfers ADC1
	uDMAChannelAttributeEnable(UDMA_CH24_ADC1_0, UDMA_ATTR_USEBURST);


    // Channel A udma control set
    uDMAChannelControlSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
    					  UDMA_SIZE_16 |
						  UDMA_SRC_INC_NONE |
						  UDMA_DST_INC_16 |
						  UDMA_ARB_8);

    uDMAChannelControlSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT,
    					  UDMA_SIZE_16 |
						  UDMA_SRC_INC_NONE |
						  UDMA_DST_INC_16 |
						  UDMA_ARB_8);

    // Channel A transfer set
    uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
    					   UDMA_MODE_PINGPONG,
						   (void *) (ADC0_BASE + ADC_O_SSFIFO0),
						   data_array1,
						   ADC_SAMPLE_BUF_SIZE);

    uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT,
    					   UDMA_MODE_PINGPONG,
						   (void *) (ADC0_BASE + ADC_O_SSFIFO0),
						   data_array2,
						   ADC_SAMPLE_BUF_SIZE);

    // Channel A udma control set  for ADC1
       uDMAChannelControlSet(UDMA_CH24_ADC1_0 | UDMA_PRI_SELECT,
       					  UDMA_SIZE_16 |
   						  UDMA_SRC_INC_NONE |
   						  UDMA_DST_INC_16 |
   						  UDMA_ARB_8);

       uDMAChannelControlSet(UDMA_CH24_ADC1_0 | UDMA_ALT_SELECT,
       					  UDMA_SIZE_16 |
   						  UDMA_SRC_INC_NONE |
   						  UDMA_DST_INC_16 |
   						  UDMA_ARB_8);

       // Channel A transfer set for ADC1
       uDMAChannelTransferSet(UDMA_CH24_ADC1_0 | UDMA_PRI_SELECT,
       					   UDMA_MODE_PINGPONG,
   						   (void *) (ADC1_BASE + ADC_O_SSFIFO0),
   						   data_array3,
   						   ADC_SAMPLE_BUF_SIZE);

       uDMAChannelTransferSet(UDMA_CH24_ADC1_0 | UDMA_ALT_SELECT,
       					   UDMA_MODE_PINGPONG,
   						   (void *) (ADC1_BASE + ADC_O_SSFIFO0),
   						   data_array4,
   						   ADC_SAMPLE_BUF_SIZE);

    /* Enable the channels */
    // Channel for ADC0
    uDMAChannelEnable(UDMA_CHANNEL_ADC0);

    // Channel for ADC1
    uDMAChannelEnable(UDMA_CH24_ADC1_0);
}


void ADC_Seq0_ISR(void)
{
GPIO_write(DebugPin3,1);
    uint32_t modePrimary;
    uint32_t modeAlternate;

    // Clear the interrupt
    ADCIntClearEx(ADC0_BASE, ADC_INT_DMA_SS0);

    // Get the mode statuses of primary and alternate control structures
    modePrimary = uDMAChannelModeGet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT);
    modeAlternate = uDMAChannelModeGet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT);

    // Reload the control structures
    if ((modePrimary == UDMA_MODE_STOP) && (modeAlternate != UDMA_MODE_STOP))
    {
        // Need to reload primary control structure
        uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
        					   UDMA_MODE_PINGPONG,
							   (void *) (ADC0_BASE + ADC_O_SSFIFO0),
							   data_array1,
							   ADC_SAMPLE_BUF_SIZE);

        uDMATransferCount++;
        if(uDMATransferCount>IGNORE_INIT_SAMPLES) Semaphore_post(data_proc1_Sem); //FIXME: Do it better, it will break down sometime....

    }
    else if ((modePrimary != UDMA_MODE_STOP) && (modeAlternate == UDMA_MODE_STOP))
    {
        // Need to reload alternate control structure
        uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT,
        					   UDMA_MODE_PINGPONG,
							   (void *) (ADC0_BASE + ADC_O_SSFIFO0),
							   data_array2,
							   ADC_SAMPLE_BUF_SIZE);

        uDMATransferCount++;
        if(uDMATransferCount>IGNORE_INIT_SAMPLES) Semaphore_post(data_proc2_Sem);
    }

#ifdef DEBUG_DMA
    else {
        // Either both still not stopped, or both stopped. This is an error
    	Log_info1("DMA - Error in uDMA control structure modes\n uDMATransferCount: %d\n", uDMATransferCount);
    	Log_info1("DMA - Primary mode: %d \n", modePrimary);
    	Log_info1("DMA - Alternate mode: %d\n", modeAlternate);
    	Log_info1("DMA - For reference: Stop: %d", UDMA_MODE_STOP);
    	Log_info1("DMA - Basic: %d", UDMA_MODE_BASIC);
    	Log_info1("DMA - Auto: %d", UDMA_MODE_AUTO);
    	Log_info1("DMA - PingPong: %d", UDMA_MODE_PINGPONG);
    	Log_info1("DMA - MemScatter: %d", UDMA_MODE_MEM_SCATTER_GATHER);
    	Log_info1("DMA - PerScatter: %d\n",UDMA_MODE_PER_SCATTER_GATHER);
    	Log_info0("DMA - Reloading Settings ...");
    	DMA_init(); // Call DMA_init again to refresh the settings, this is very important in DEBUG mode!
    }
#endif

GPIO_write(DebugPin3,0);
}

void ADC_Seq1_ISR(void)
{
//GPIO_write(DebugPin3,1);

    uint32_t modePrimary;
    uint32_t modeAlternate;

    // Clear the interrupt
    ADCIntClearEx(ADC1_BASE, ADC_INT_DMA_SS0);


    // Get the mode statuses of primary and alternate control structures
    modePrimary = uDMAChannelModeGet(UDMA_CH24_ADC1_0 | UDMA_PRI_SELECT);
    modeAlternate = uDMAChannelModeGet(UDMA_CH24_ADC1_0 | UDMA_ALT_SELECT);

    // Reload the control structures
    if ((modePrimary == UDMA_MODE_STOP) && (modeAlternate != UDMA_MODE_STOP))
    {
        // Need to reload primary control structure
        uDMAChannelTransferSet(UDMA_CH24_ADC1_0 | UDMA_PRI_SELECT,
        					   UDMA_MODE_PINGPONG,
							   (void *) (ADC1_BASE + ADC_O_SSFIFO0),
							   data_array3,
							   ADC_SAMPLE_BUF_SIZE);

        uDMATransferCount2++;
        if(uDMATransferCount2>IGNORE_INIT_SAMPLES) Semaphore_post(data_proc3_Sem);

    }
    else if ((modePrimary != UDMA_MODE_STOP) && (modeAlternate == UDMA_MODE_STOP))
    {
        // Need to reload alternate control structure
        uDMAChannelTransferSet(UDMA_CH24_ADC1_0 | UDMA_ALT_SELECT,
        					   UDMA_MODE_PINGPONG,
							   (void *) (ADC1_BASE + ADC_O_SSFIFO0),
							   data_array4,
							   ADC_SAMPLE_BUF_SIZE);

        uDMATransferCount2++;
        if(uDMATransferCount>IGNORE_INIT_SAMPLES)Semaphore_post(data_proc4_Sem);
    }
#ifdef DEBUG_DMA
    else {
        // Either both still not stopped, or both stopped. This is an error
    	Log_info1("DMA - Error in uDMA control structure modes\n uDMATransferCount: %d\n", uDMATransferCount);
    	Log_info1("DMA - Primary mode: %d \n", modePrimary);
    	Log_info1("DMA - Alternate mode: %d\n", modeAlternate);
    	Log_info1("DMA - For reference: Stop: %d", UDMA_MODE_STOP);
    	Log_info1("DMA - Basic: %d", UDMA_MODE_BASIC);
    	Log_info1("DMA - Auto: %d", UDMA_MODE_AUTO);
    	Log_info1("DMA - PingPong: %d", UDMA_MODE_PINGPONG);
    	Log_info1("DMA - MemScatter: %d", UDMA_MODE_MEM_SCATTER_GATHER);
    	Log_info1("DMA - PerScatter: %d\n",UDMA_MODE_PER_SCATTER_GATHER);
    	Log_info0("DMA - Reloading Settings ...");
    	DMA_init(); // Call DMA_init again to refresh the settings, this is very important in DEBUG mode!
    }
#endif

//GPIO_write(DebugPin3,0);
}
