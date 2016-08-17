/*TODO: Update this discription
 * Clean code Options.
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

#include "arm_math.h"	// Use CMSIS ARM library for fast math operation.

#define TCPPACKETSIZE 256
#define NUMTCPWORKERS 3

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

void ADC_Seq0_ISR();	//Interrupt service of ADC Sequencer 0
void ADC_Seq1_ISR();	//Interrupt service of ADC Sequencer 1

void InitSamples();		//Initialize samples
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
				j+=NUMBER_OF_CH_ADC;
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
				j+=NUMBER_OF_CH_ADC;
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
				j+=NUMBER_OF_CH_ADC;
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
					j+=NUMBER_OF_CH_ADC;
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
