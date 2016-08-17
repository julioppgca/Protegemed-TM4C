/*TODO: Update this discription
 *
 */

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
#include <ptgmed_inc/ADC_pinout.h>
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

#include "arm_math.h"	// Use CMSIS ARM library for fast math operation.

#define TCPPACKETSIZE 256
#define NUMTCPWORKERS 3

#define CLK_FREQ            120000000	// Main CPU clock -> 120MHz
#define SAMPLE_FREQ         15360		// 15360KHz, ADC Samples per Second -> 1MSPS
#define CH_SAMPLE_NUMBER	256			// Samples per channel
#define ADC_SAMPLE_BUF_SIZE 1024 		// Max uDMA transfer buffer, sucks... NUMBER_OF_CHANNELS * CH_SAMPLE_NUMBER	// Buffer size for uDMA
#define OVER_SAMPLE_VALUE	16			// Over sample value -> 64 avereage samples in 1MSPS, sample every 64us.
#define ADC_CHANNEL_OFFSET  2048		// Offset of the ADC channel -> 4096/2 = 2048
#define ADC_INPUT_SCALE		3.3/4095	// Vmax / ADC_Resolution -> 3.3V/(2^12-1)
#define NUMBER_OF_CHANNELS	8			// Max. 4

#define IGNORE_INIT_SAMPLES	2			// Leave DMA and ADC initialize, 2 is good enough
uint32_t ui32Ignore=0;

#define DEBUG_DMA

//uint32_t udmaCtrlTable[1024/sizeof(uint32_t)]__attribute__((aligned(1024))); // uDMA control table variable
uint32_t udmaCtrlTable[1024]__attribute__((aligned(1024))); // uDMA control table variable
int16_t data_array1[ADC_SAMPLE_BUF_SIZE]={}; // Init to zero in all positions
int16_t data_array2[ADC_SAMPLE_BUF_SIZE]={};
int16_t data_array3[ADC_SAMPLE_BUF_SIZE]={};
int16_t data_array4[ADC_SAMPLE_BUF_SIZE]={};

float32_t	data_proc0[CH_SAMPLE_NUMBER]={}; // Process variable first cycle
float32_t	data_proc1[CH_SAMPLE_NUMBER]={}; // Process variable second cycle
float32_t	data_proc2[CH_SAMPLE_NUMBER]={}; // Process variable second cycle
float32_t	data_proc3[CH_SAMPLE_NUMBER]={}; // Process variable second cycle
float32_t	data_proc4[CH_SAMPLE_NUMBER]={}; // Process variable first cycle
float32_t	data_proc5[CH_SAMPLE_NUMBER]={}; // Process variable second cycle
float32_t	data_proc6[CH_SAMPLE_NUMBER]={}; // Process variable second cycle
float32_t	data_proc7[CH_SAMPLE_NUMBER]={}; // Process variable second cycle

float32_t	data_proc8[CH_SAMPLE_NUMBER]={}; // Process variable first cycle
float32_t	data_proc9[CH_SAMPLE_NUMBER]={}; // Process variable second cycle
float32_t	data_proc10[CH_SAMPLE_NUMBER]={}; // Process variable second cycle
float32_t	data_proc11[CH_SAMPLE_NUMBER]={}; // Process variable second cycle
float32_t	data_proc12[CH_SAMPLE_NUMBER]={}; // Process variable first cycle
float32_t	data_proc13[CH_SAMPLE_NUMBER]={}; // Process variable second cycle
float32_t	data_proc14[CH_SAMPLE_NUMBER]={}; // Process variable second cycle
float32_t	data_proc15[CH_SAMPLE_NUMBER]={}; // Process variable second cycle

float32_t data_proc0_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc1_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc2_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc3_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc4_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc5_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc6_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc7_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis

float32_t data_proc8_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc9_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc10_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc11_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc12_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc13_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc14_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis
float32_t data_proc15_mag[CH_SAMPLE_NUMBER/2]; // Magnitude of fft analysis

float32_t rms0=0;
float32_t rms1=0;
float32_t rms2=0;
float32_t rms3=0;
float32_t rms4=0;
float32_t rms5=0;
float32_t rms6=0;
float32_t rms7=0;

float32_t rms8=0;
float32_t rms9=0;
float32_t rms10=0;
float32_t rms11=0;
float32_t rms12=0;
float32_t rms13=0;
float32_t rms14=0;
float32_t rms15=0;

uint16_t buffer=0;

uint32_t uDMATransferCount = 0;
uint32_t uDMATransferCount2 = 0;

float32_t	wave1[CH_SAMPLE_NUMBER]={};
float32_t	wave2[CH_SAMPLE_NUMBER]={};

void ADC_Seq0_ISR();
void ADC_Seq1_ISR();
void TIMER_init();
void ADC_init();
void DMA_init();
void InitSamples();
void RMSCalc_Task();
void FFTCalc_Task();
void HeartBeat_Idle();
Void tcpWorker(UArg arg0, UArg arg1);
Void tcpHandler(UArg arg0, UArg arg1);

int main(void)
{

    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initEMAC();

    GPIO_write(Board_LED1,1);

    /* Start BIOS */
    BIOS_start();

    return (0);
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
    if ((modePrimary == UDMA_MODE_STOP) && (modeAlternate != UDMA_MODE_STOP)) {
        // Need to reload primary control structure
        uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_PRI_SELECT,
        					   UDMA_MODE_PINGPONG,
							   (void *) (ADC0_BASE + ADC_O_SSFIFO0),
							   data_array1,
							   ADC_SAMPLE_BUF_SIZE);

        uDMATransferCount++;
        buffer=1;
        if(uDMATransferCount>IGNORE_INIT_SAMPLES) Semaphore_post(data_proc1_Sem); //FIXME: Do it better, it will break down sometime....

    } else if ((modePrimary != UDMA_MODE_STOP) && (modeAlternate == UDMA_MODE_STOP)) {
        // Need to reload alternate control structure
        uDMAChannelTransferSet(UDMA_CHANNEL_ADC0 | UDMA_ALT_SELECT,
        					   UDMA_MODE_PINGPONG,
							   (void *) (ADC0_BASE + ADC_O_SSFIFO0),
							   data_array2,
							   ADC_SAMPLE_BUF_SIZE);

        uDMATransferCount++;
        buffer=2;
        if(uDMATransferCount>IGNORE_INIT_SAMPLES) Semaphore_post(data_proc2_Sem);
        //for(i=0;i<ADC_SAMPLE_BUF_SIZE;i++) data_proc2[i]=data_array2[i]-ADC_CHANNEL_OFFSET;
       // Swi_post(RMSCalc_Handle);

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
    	DMA_init(); // Call DMA_init again to refresh the settings...
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
    if ((modePrimary == UDMA_MODE_STOP) && (modeAlternate != UDMA_MODE_STOP)) {
        // Need to reload primary control structure
        uDMAChannelTransferSet(UDMA_CH24_ADC1_0 | UDMA_PRI_SELECT,
        					   UDMA_MODE_PINGPONG,
							   (void *) (ADC1_BASE + ADC_O_SSFIFO0),
							   data_array3,
							   ADC_SAMPLE_BUF_SIZE);

        uDMATransferCount2++;
        buffer=3;
       if(uDMATransferCount2>IGNORE_INIT_SAMPLES) Semaphore_post(data_proc3_Sem);

    } else if ((modePrimary != UDMA_MODE_STOP) && (modeAlternate == UDMA_MODE_STOP)) {
        // Need to reload alternate control structure
        uDMAChannelTransferSet(UDMA_CH24_ADC1_0 | UDMA_ALT_SELECT,
        					   UDMA_MODE_PINGPONG,
							   (void *) (ADC1_BASE + ADC_O_SSFIFO0),
							   data_array4,
							   ADC_SAMPLE_BUF_SIZE);

        uDMATransferCount2++;
        buffer=4;
        if(uDMATransferCount>IGNORE_INIT_SAMPLES)Semaphore_post(data_proc4_Sem);
        //for(i=0;i<ADC_SAMPLE_BUF_SIZE;i++) data_proc2[i]=data_array2[i]-ADC_CHANNEL_OFFSET;
       // Swi_post(RMSCalc_Handle);

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
    	DMA_init(); // Call DMA_init again to refresh the settings...
    }
#endif

//GPIO_write(DebugPin3,0);
}

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
    TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC_UP); // 12000 -> 78.1hz //14000 -> 67.6 15625->60.2
    TimerLoadSet(TIMER0_BASE, TIMER_A, (ui32ClockFreq / SAMPLE_FREQ)); // TODO: REvisar isso. (OK 7800)

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

void InitSamples(void)
{
	IntMasterDisable();
	ADC_init();
    DMA_init();
    TIMER_init();
    IntMasterEnable();
}


void RMSCalc_Task(void)
{
	while(1)
	{
		Semaphore_pend(data_proc1_Sem, BIOS_WAIT_FOREVER); // First half 1st wave
	GPIO_write(DebugPin1,1);

		uint32_t i,j;

			for(i=0,j=0;i<CH_SAMPLE_NUMBER/2;i++)
				{
				data_proc0[i]=((float)data_array1[j]-ADC_CHANNEL_OFFSET) ; wave1[i]=data_proc0[i];
				data_proc1[i]=((float)data_array1[j+1]-ADC_CHANNEL_OFFSET) ;
				data_proc2[i]=((float)data_array1[j+2]-ADC_CHANNEL_OFFSET) ;
				data_proc3[i]=((float)data_array1[j+3]-ADC_CHANNEL_OFFSET) ;
				data_proc4[i]=((float)data_array1[j+4]-ADC_CHANNEL_OFFSET) ;
				data_proc5[i]=((float)data_array1[j+5]-ADC_CHANNEL_OFFSET) ;
				data_proc6[i]=((float)data_array1[j+6]-ADC_CHANNEL_OFFSET) ;
				data_proc7[i]=((float)data_array1[j+7]-ADC_CHANNEL_OFFSET) ;
				j+=NUMBER_OF_CHANNELS;
				}
	GPIO_write(DebugPin1,0);

		Semaphore_pend(data_proc3_Sem, BIOS_WAIT_FOREVER); // First half 2nd wave

	GPIO_write(DebugPin2,1);
			for(i=0,j=0;i<CH_SAMPLE_NUMBER/2;i++)
				{
				data_proc8[i]=((float)data_array3[j]-ADC_CHANNEL_OFFSET) ; 	//wave2[i]=data_proc8[i];
				data_proc9[i]=((float)data_array3[j+1]-ADC_CHANNEL_OFFSET) ; wave2[i]=data_proc9[i];
				data_proc10[i]=((float)data_array3[j+2]-ADC_CHANNEL_OFFSET) ;
				data_proc11[i]=((float)data_array3[j+3]-ADC_CHANNEL_OFFSET) ;
				data_proc12[i]=((float)data_array3[j+4]-ADC_CHANNEL_OFFSET) ;
				data_proc13[i]=((float)data_array3[j+5]-ADC_CHANNEL_OFFSET) ;
				data_proc14[i]=((float)data_array3[j+6]-ADC_CHANNEL_OFFSET) ;
				data_proc15[i]=((float)data_array3[j+7]-ADC_CHANNEL_OFFSET) ;
				j+=NUMBER_OF_CHANNELS;
				}
	GPIO_write(DebugPin2,0);

		Semaphore_pend(data_proc2_Sem, BIOS_WAIT_FOREVER); // Second half 1st wave, calculate RMS

	GPIO_write(DebugPin1,1);
		for(i=128,j=0;i<CH_SAMPLE_NUMBER;i++)
				{
				data_proc0[i]=((float)data_array2[j]-ADC_CHANNEL_OFFSET) ;wave1[i]=data_proc0[i];
				data_proc1[i]=((float)data_array2[j+1]-ADC_CHANNEL_OFFSET) ;
				data_proc2[i]=((float)data_array2[j+2]-ADC_CHANNEL_OFFSET) ;
				data_proc3[i]=((float)data_array2[j+3]-ADC_CHANNEL_OFFSET) ;
				data_proc4[i]=((float)data_array2[j+4]-ADC_CHANNEL_OFFSET) ;
				data_proc5[i]=((float)data_array2[j+5]-ADC_CHANNEL_OFFSET) ;
				data_proc6[i]=((float)data_array2[j+6]-ADC_CHANNEL_OFFSET) ;
				data_proc7[i]=((float)data_array2[j+7]-ADC_CHANNEL_OFFSET) ;
				j+=NUMBER_OF_CHANNELS;
				}

		// Use CMSIS to multiply float, way faster....
		arm_scale_f32(data_proc0, ADC_INPUT_SCALE, data_proc0, CH_SAMPLE_NUMBER);
		arm_scale_f32(data_proc1, ADC_INPUT_SCALE, data_proc1, CH_SAMPLE_NUMBER);
		arm_scale_f32(data_proc2, ADC_INPUT_SCALE, data_proc2, CH_SAMPLE_NUMBER);
		arm_scale_f32(data_proc3, ADC_INPUT_SCALE, data_proc3, CH_SAMPLE_NUMBER);
		arm_scale_f32(data_proc4, ADC_INPUT_SCALE, data_proc4, CH_SAMPLE_NUMBER);
		arm_scale_f32(data_proc5, ADC_INPUT_SCALE, data_proc5, CH_SAMPLE_NUMBER);
		arm_scale_f32(data_proc6, ADC_INPUT_SCALE, data_proc6, CH_SAMPLE_NUMBER);
		arm_scale_f32(data_proc7, ADC_INPUT_SCALE, data_proc7, CH_SAMPLE_NUMBER);

		// Calculate RMS value
		arm_rms_f32(data_proc0,CH_SAMPLE_NUMBER,&rms0);
		arm_rms_f32(data_proc1,CH_SAMPLE_NUMBER,&rms1);
		arm_rms_f32(data_proc2,CH_SAMPLE_NUMBER,&rms2);
		arm_rms_f32(data_proc3,CH_SAMPLE_NUMBER,&rms3);
		arm_rms_f32(data_proc4,CH_SAMPLE_NUMBER,&rms4);
		arm_rms_f32(data_proc5,CH_SAMPLE_NUMBER,&rms5);
		arm_rms_f32(data_proc6,CH_SAMPLE_NUMBER,&rms6);
		arm_rms_f32(data_proc7,CH_SAMPLE_NUMBER,&rms7);

	GPIO_write(DebugPin1,0);

		// Ready to FFT
		Semaphore_post(data_proc1_fft_Sem);


			Semaphore_pend(data_proc4_Sem, BIOS_WAIT_FOREVER); // Second half 2nd wave, calculate RMS

		GPIO_write(DebugPin2,1);
			for(i=128,j=0;i<CH_SAMPLE_NUMBER;i++)
					{
					data_proc8[i]=((float)data_array4[j]-ADC_CHANNEL_OFFSET);		// wave2[i]=data_proc8[i];
					data_proc9[i]=((float)data_array4[j+1]-ADC_CHANNEL_OFFSET);		wave2[i]=data_proc9[i];
					data_proc10[i]=((float)data_array4[j+2]-ADC_CHANNEL_OFFSET);
					data_proc11[i]=((float)data_array4[j+3]-ADC_CHANNEL_OFFSET);
					data_proc12[i]=((float)data_array4[j+4]-ADC_CHANNEL_OFFSET);
					data_proc13[i]=((float)data_array4[j+5]-ADC_CHANNEL_OFFSET);
					data_proc14[i]=((float)data_array4[j+6]-ADC_CHANNEL_OFFSET);
					data_proc15[i]=((float)data_array4[j+7]-ADC_CHANNEL_OFFSET);
					j+=NUMBER_OF_CHANNELS;
					}

			// Use CMSIS to multiply float, way faster....
			arm_scale_f32(data_proc8, ADC_INPUT_SCALE, data_proc8, CH_SAMPLE_NUMBER);
			arm_scale_f32(data_proc9, ADC_INPUT_SCALE, data_proc9, CH_SAMPLE_NUMBER);
			arm_scale_f32(data_proc10, ADC_INPUT_SCALE, data_proc10, CH_SAMPLE_NUMBER);
			arm_scale_f32(data_proc11, ADC_INPUT_SCALE, data_proc11, CH_SAMPLE_NUMBER);
			arm_scale_f32(data_proc12, ADC_INPUT_SCALE, data_proc12, CH_SAMPLE_NUMBER);
			arm_scale_f32(data_proc13, ADC_INPUT_SCALE, data_proc13, CH_SAMPLE_NUMBER);
			arm_scale_f32(data_proc14, ADC_INPUT_SCALE, data_proc14, CH_SAMPLE_NUMBER);
			arm_scale_f32(data_proc15, ADC_INPUT_SCALE, data_proc15, CH_SAMPLE_NUMBER);

			// Calculate RMS value
			arm_rms_f32(data_proc8,CH_SAMPLE_NUMBER,&rms8);
			arm_rms_f32(data_proc9,CH_SAMPLE_NUMBER,&rms9);
			arm_rms_f32(data_proc10,CH_SAMPLE_NUMBER,&rms10);
			arm_rms_f32(data_proc11,CH_SAMPLE_NUMBER,&rms11);
			arm_rms_f32(data_proc12,CH_SAMPLE_NUMBER,&rms12);
			arm_rms_f32(data_proc13,CH_SAMPLE_NUMBER,&rms13);
			arm_rms_f32(data_proc14,CH_SAMPLE_NUMBER,&rms14);
			arm_rms_f32(data_proc15,CH_SAMPLE_NUMBER,&rms15);

			// Ready to FFT
			Semaphore_post(data_proc3_fft_Sem);

		GPIO_write(DebugPin2,0);
	}
}


void FFTCalc_Task()
{
	while(1)
	{
		Semaphore_pend(data_proc1_fft_Sem, BIOS_WAIT_FOREVER);
		GPIO_write(DebugPin1,1);
		arm_rfft_fast_instance_f32 s;

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc0,data_proc0,0);
		arm_cmplx_mag_f32(data_proc0,data_proc0_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc1,data_proc1,0);
		arm_cmplx_mag_f32(data_proc1,data_proc1_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc2,data_proc2,0);
		arm_cmplx_mag_f32(data_proc2,data_proc2_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc3,data_proc3,0);
		arm_cmplx_mag_f32(data_proc3,data_proc3_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc4,data_proc4,0);
		arm_cmplx_mag_f32(data_proc4,data_proc4_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc5,data_proc5,0);
		arm_cmplx_mag_f32(data_proc5,data_proc5_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc6,data_proc6,0);
		arm_cmplx_mag_f32(data_proc6,data_proc6_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc7,data_proc7,0);
		arm_cmplx_mag_f32(data_proc7,data_proc7_mag,CH_SAMPLE_NUMBER/2);

		GPIO_write(DebugPin1,0);
		Semaphore_pend(data_proc3_fft_Sem, BIOS_WAIT_FOREVER);
		GPIO_write(DebugPin2,1);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc8,data_proc8,0);
		arm_cmplx_mag_f32(data_proc8,data_proc8_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc9,data_proc9,0);
		arm_cmplx_mag_f32(data_proc9,data_proc9_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc10,data_proc10,0);
		arm_cmplx_mag_f32(data_proc10,data_proc10_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc11,data_proc11,0);
		arm_cmplx_mag_f32(data_proc11,data_proc11_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc12,data_proc12,0);
		arm_cmplx_mag_f32(data_proc12,data_proc12_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc13,data_proc13,0);
		arm_cmplx_mag_f32(data_proc13,data_proc13_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc14,data_proc14,0);
		arm_cmplx_mag_f32(data_proc14,data_proc14_mag,CH_SAMPLE_NUMBER/2);

		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
		arm_rfft_fast_f32(&s,data_proc15,data_proc15,0);
		arm_cmplx_mag_f32(data_proc15,data_proc15_mag,CH_SAMPLE_NUMBER/2);

		Semaphore_post(fft_end_Sem);
		GPIO_write(DebugPin2,0);

		if(uDMATransferCount>7)
		{
			GPIO_write(DebugPin2,0);
		}
	}
}

void HeartBeat_Idle(void)
{
	GPIO_toggle(Board_LED1);
	SysCtlDelay(1000000);
}


/*
 *  ======== tcpWorker ========
 *  Task to handle TCP connection. Can be multiple Tasks running
 *  this function.
 */
Void tcpWorker(UArg arg0, UArg arg1)
{
    int  clientfd = (int)arg0;
//    int  bytesRcvd;
//    int  bytesSent;
    char buffer[TCPPACKETSIZE];
    char helloTM4C[]="\n*-----------------------*\n   Hello from TM4C   \n ";

    System_printf("tcpWorker: start clientfd = 0x%x\n", clientfd);

    recv(clientfd, buffer, TCPPACKETSIZE, 0);

    if(buffer[0]=='S')
    {

        send(clientfd, helloTM4C, sizeof(helloTM4C), 0);
    }
    else
    {
        send(clientfd, "Comando Invalido!!!", 19, 0);
    }


//    while ((bytesRcvd = recv(clientfd, buffer, TCPPACKETSIZE, 0)) > 0) {
//        bytesSent = send(clientfd, buffer, bytesRcvd, 0);
//        if (bytesSent < 0 || bytesSent != bytesRcvd) {
//            System_printf("Error: send failed.\n");
//            break;
//        }
//    }
    System_printf("tcpWorker stop clientfd = 0x%x\n", clientfd);

    close(clientfd);
}

/*
 *  ======== tcpHandler ========
 *  Creates new Task to handle new TCP connections.
 */
Void tcpHandler(UArg arg0, UArg arg1)
{
    int                status;
    int                clientfd;
    int                server;
    struct sockaddr_in localAddr;
    struct sockaddr_in clientAddr;
    int                optval;
    int                optlen = sizeof(optval);
    socklen_t          addrlen = sizeof(clientAddr);
    Task_Handle        taskHandle;
    Task_Params        taskParams;
    Error_Block        eb;

    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) {
        System_printf("Error: socket not created.\n");
        goto shutdown;
    }


    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(arg0);

    status = bind(server, (struct sockaddr *)&localAddr, sizeof(localAddr));
    if (status == -1) {
        System_printf("Error: bind failed.\n");
        goto shutdown;
    }

    status = listen(server, NUMTCPWORKERS);
    if (status == -1) {
        System_printf("Error: listen failed.\n");
        goto shutdown;
    }

    optval = 1;
    if (setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
        System_printf("Error: setsockopt failed\n");
        goto shutdown;
    }

    while ((clientfd =
            accept(server, (struct sockaddr *)&clientAddr, &addrlen)) != -1) {

    	Semaphore_pend(fft_end_Sem, BIOS_WAIT_FOREVER);

        System_printf("tcpHandler: Creating thread clientfd = %d\n", clientfd);

        /* Init the Error_Block */
        Error_init(&eb);

        /* Initialize the defaults and set the parameters. */
        Task_Params_init(&taskParams);
        taskParams.arg0 = (UArg)clientfd;
        taskParams.stackSize = 1280;
        taskHandle = Task_create((Task_FuncPtr)tcpWorker, &taskParams, &eb);
        if (taskHandle == NULL) {
            System_printf("Error: Failed to create new Task\n");
            close(clientfd);
        }

        /* addrlen is a value-result param, must reset for next accept call */
        addrlen = sizeof(clientAddr);
    }

    System_printf("Error: accept failed.\n");

shutdown:
    if (server > 0) {
        close(server);
    }
}
