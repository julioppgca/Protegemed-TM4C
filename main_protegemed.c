/*TODO: Update this discription
 * Clean code Options.
 */

#include <ptgmed_inc/system.h>

#define DEBUG_DMA 		//Comment this if not running DEBUG mode.

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

uint32_t uDMATransferCount = 0;
uint32_t uDMATransferCount2 = 0;

float32_t	wave1[CH_SAMPLE_NUMBER]={};
float32_t	wave2[CH_SAMPLE_NUMBER]={};


void InitSamples();		//Initialize samples
void RMSCalc_Task();	//Perform RMS Calculation
void FFTCalc_Task();	//Perform FFT Calculation
void HeartBeat_Idle();	//If there is nothing better to do, blynk the user led!!!

int main(void)
{

    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initEMAC();


    /* Start BIOS */
    BIOS_start();

    return (0);
}

void InitSamples(void)
{
	ADC_init();
    DMA_init();
    TIMER_init();
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
