/*TODO: Update this description ASAP
 * Clean code Options.
 *
 * CMSIS DSP Ref. : http://www.keil.com/pack/doc/CMSIS/DSP/html/index.html
 *
 */

#include <includes_protegemd/system.h>


//#define USE_ETH_BOOTLOADER
#define DEBUG_DMA 		//Comment this if not running DEBUG mode.

/*
 *  plug definition struct, test only.
 */

outlet Outlet_1, Outlet_2, Outlet_3, Outlet_4, Outlet_5, Outlet_6;
panel_voltages Panel_Voltages;

uint32_t udmaCtrlTable[1024]__attribute__((aligned(1024))); // uDMA control table variable
int16_t data_array1[ADC_SAMPLE_BUF_SIZE] = { }; // Init to zero in all positions
int16_t data_array2[ADC_SAMPLE_BUF_SIZE] = { };
int16_t data_array3[ADC_SAMPLE_BUF_SIZE] = { };
int16_t data_array4[ADC_SAMPLE_BUF_SIZE] = { };

uint32_t uDMATransferCount = 0;
uint32_t uDMATransferCount2 = 0;

// FIXME: Debug purpose only, remove this.
float32_t wave1[256] = { };
float32_t wave2[256] = { };
float32_t wave3[256] = { };
uint32_t rfid=0;


void InitSamples();		//Initialize samples
void RMSCalc_Task();	//Perform RMS Calculation
void FFTCalc_Task();	//Perform FFT Calculation
void Read_RFID();
void HeartBeat_Idle();//If there is nothing better to do, blynk the user led!!!
void RFID_SCIO(unsigned int index);

int main(void){
    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initEMAC();
    Board_initUART();
    GPIO_write(RFIDPinTXCT, 1);

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
    while (1)
    {
        Semaphore_pend(data_proc1_Sem, BIOS_WAIT_FOREVER); // First half 1st wave
        GPIO_write(DebugPin1, 1);

        uint32_t i, j;
        for (i = 0, j = 0; i < CH_SAMPLE_NUMBER / 2; i++)
        {
            Outlet_1.dif_samples[i] = ((float) data_array1[j]);
            Outlet_1.ph_samples[i] = ((float) data_array1[j + 1]);

            Outlet_2.dif_samples[i] = ((float) data_array1[j + 2]);
            Outlet_2.ph_samples[i] = ((float) data_array1[j + 3]);

            Outlet_3.dif_samples[i] = ((float) data_array1[j + 4]);
            Outlet_3.ph_samples[i] = ((float) data_array1[j + 5]);

            Outlet_4.dif_samples[i] = ((float) data_array1[j + 6]);
            Outlet_4.ph_samples[i] = ((float) data_array1[j + 7]);

            j += NUMBER_OF_CH_ADC;
        }
        GPIO_write(DebugPin1, 0);

        Semaphore_pend(data_proc3_Sem, BIOS_WAIT_FOREVER); // First half 2nd wave

        GPIO_write(DebugPin2, 1);
        for (i = 0, j = 0; i < CH_SAMPLE_NUMBER / 2; i++)
        {
            Outlet_5.dif_samples[i] = ((float) data_array3[j]);
            Outlet_5.ph_samples[i] = ((float) data_array3[j + 1]);

            Outlet_6.dif_samples[i] = ((float) data_array3[j + 2]);
            Outlet_6.ph_samples[i] = ((float) data_array3[j + 3]);

            Panel_Voltages.voltage_samples_L1[i] = ((float) data_array3[j + 4]);
            Panel_Voltages.voltage_samples_L2[i] = ((float) data_array3[j + 5]);
            Panel_Voltages.voltage_samples_L3[i] = ((float) data_array3[j + 6]);
            //Free_Channel[i]=((float)data_array3[j+7]-ADC_CHANNEL_OFFSET) ;
            j += NUMBER_OF_CH_ADC;
        }
        GPIO_write(DebugPin2, 0);

        Semaphore_pend(data_proc2_Sem, BIOS_WAIT_FOREVER); // Second half 1st wave, calculate RMS

        GPIO_write(DebugPin1, 1);
        for (i = 128, j = 0; i < CH_SAMPLE_NUMBER; i++)
        {
            Outlet_1.dif_samples[i] = ((float) data_array2[j]);
            Outlet_1.ph_samples[i] = ((float) data_array2[j + 1]);

            Outlet_2.dif_samples[i] = ((float) data_array2[j + 2]);
            Outlet_2.ph_samples[i] = ((float) data_array2[j + 3]);

            Outlet_3.dif_samples[i] = ((float) data_array2[j + 4]);
            Outlet_3.ph_samples[i] = ((float) data_array2[j + 5]);

            Outlet_4.dif_samples[i] = ((float) data_array2[j + 6]);
            Outlet_4.ph_samples[i] = ((float) data_array2[j + 7]);

            j += NUMBER_OF_CH_ADC;
        }
        // Use CMSIS to apply offset
        arm_offset_f32(Outlet_1.dif_samples, -ADC_CHANNEL_OFFSET,
                       Outlet_1.dif_samples, CH_SAMPLE_NUMBER);
        arm_offset_f32(Outlet_1.ph_samples, -ADC_CHANNEL_OFFSET,
                       Outlet_1.ph_samples, CH_SAMPLE_NUMBER);
        arm_offset_f32(Outlet_2.dif_samples, -ADC_CHANNEL_OFFSET,
                       Outlet_2.dif_samples, CH_SAMPLE_NUMBER);
        arm_offset_f32(Outlet_2.ph_samples, -ADC_CHANNEL_OFFSET,
                       Outlet_2.ph_samples, CH_SAMPLE_NUMBER);
        arm_offset_f32(Outlet_3.dif_samples, -ADC_CHANNEL_OFFSET,
                       Outlet_3.dif_samples, CH_SAMPLE_NUMBER);
        arm_offset_f32(Outlet_3.ph_samples, -ADC_CHANNEL_OFFSET,
                       Outlet_3.ph_samples, CH_SAMPLE_NUMBER);
        arm_offset_f32(Outlet_4.dif_samples, -ADC_CHANNEL_OFFSET,
                       Outlet_4.dif_samples, CH_SAMPLE_NUMBER);
        arm_offset_f32(Outlet_4.ph_samples, -ADC_CHANNEL_OFFSET,
                       Outlet_4.ph_samples, CH_SAMPLE_NUMBER);

        // TODO: Debug option to visualize waves, remove this...
        arm_copy_f32(Outlet_1.dif_samples, wave1, CH_SAMPLE_NUMBER);
        arm_copy_f32(Outlet_1.ph_samples, wave2, CH_SAMPLE_NUMBER);

        // Use CMSIS to multiply float, way faster....
        arm_scale_f32(Outlet_1.dif_samples, ADC_INPUT_SCALE,
                      Outlet_1.dif_samples, CH_SAMPLE_NUMBER);
        arm_scale_f32(Outlet_1.ph_samples, ADC_INPUT_SCALE, Outlet_1.ph_samples,
                      CH_SAMPLE_NUMBER);
        arm_scale_f32(Outlet_2.dif_samples, ADC_INPUT_SCALE,
                      Outlet_2.dif_samples, CH_SAMPLE_NUMBER);
        arm_scale_f32(Outlet_2.ph_samples, ADC_INPUT_SCALE, Outlet_2.ph_samples,
                      CH_SAMPLE_NUMBER);
        arm_scale_f32(Outlet_3.dif_samples, ADC_INPUT_SCALE,
                      Outlet_3.dif_samples, CH_SAMPLE_NUMBER);
        arm_scale_f32(Outlet_3.ph_samples, ADC_INPUT_SCALE, Outlet_3.ph_samples,
                      CH_SAMPLE_NUMBER);
        arm_scale_f32(Outlet_4.dif_samples, ADC_INPUT_SCALE,
                      Outlet_4.dif_samples, CH_SAMPLE_NUMBER);
        arm_scale_f32(Outlet_4.ph_samples, ADC_INPUT_SCALE, Outlet_4.ph_samples,
                      CH_SAMPLE_NUMBER);

        // Calculate RMS value
        arm_rms_f32(Outlet_1.dif_samples, CH_SAMPLE_NUMBER, &Outlet_1.dif_rms); //&rms0);
        arm_rms_f32(Outlet_1.ph_samples, CH_SAMPLE_NUMBER, &Outlet_1.ph_rms);
        arm_rms_f32(Outlet_2.dif_samples, CH_SAMPLE_NUMBER, &Outlet_2.dif_rms);
        arm_rms_f32(Outlet_2.ph_samples, CH_SAMPLE_NUMBER, &Outlet_2.ph_rms);
        arm_rms_f32(Outlet_3.dif_samples, CH_SAMPLE_NUMBER, &Outlet_3.dif_rms);
        arm_rms_f32(Outlet_3.ph_samples, CH_SAMPLE_NUMBER, &Outlet_3.ph_rms);
        arm_rms_f32(Outlet_4.dif_samples, CH_SAMPLE_NUMBER, &Outlet_4.dif_rms);
        arm_rms_f32(Outlet_5.ph_samples, CH_SAMPLE_NUMBER, &Outlet_4.ph_rms);

        GPIO_write(DebugPin1, 0);

        // Ready to FFT
        Semaphore_post(data_proc1_fft_Sem);

        Semaphore_pend(data_proc4_Sem, BIOS_WAIT_FOREVER); // Second half 2nd wave, calculate RMS

        GPIO_write(DebugPin2, 1);
        for (i = 128, j = 0; i < CH_SAMPLE_NUMBER; i++)
        {
            Outlet_5.dif_samples[i] = ((float) data_array4[j]);
            Outlet_5.ph_samples[i] = ((float) data_array4[j + 1]);
            Outlet_6.dif_samples[i] = ((float) data_array4[j + 2]);
            Outlet_6.ph_samples[i] = ((float) data_array4[j + 3]);
            Panel_Voltages.voltage_samples_L1[i] = ((float) data_array4[j + 4]);
            Panel_Voltages.voltage_samples_L2[i] = ((float) data_array4[j + 5]);
            Panel_Voltages.voltage_samples_L3[i] = ((float) data_array4[j + 6]);
            //Free_Channel[i]=((float)data_array4[j+7]-ADC_CHANNEL_OFFSET);
            j += NUMBER_OF_CH_ADC;
        }

        // Use CMSIS to apply offset
        arm_offset_f32(Outlet_5.dif_samples, -ADC_CHANNEL_OFFSET,
                       Outlet_5.dif_samples, CH_SAMPLE_NUMBER);
        arm_offset_f32(Outlet_5.ph_samples, -ADC_CHANNEL_OFFSET,
                       Outlet_5.ph_samples, CH_SAMPLE_NUMBER);
        arm_offset_f32(Outlet_6.dif_samples, -ADC_CHANNEL_OFFSET,
                       Outlet_6.dif_samples, CH_SAMPLE_NUMBER);
        arm_offset_f32(Outlet_6.ph_samples, -ADC_CHANNEL_OFFSET,
                       Outlet_6.ph_samples, CH_SAMPLE_NUMBER);
        arm_offset_f32(Panel_Voltages.voltage_samples_L1, -ADC_CHANNEL_OFFSET,
                       Panel_Voltages.voltage_samples_L1, CH_SAMPLE_NUMBER);
        arm_offset_f32(Panel_Voltages.voltage_samples_L2, -ADC_CHANNEL_OFFSET,
                       Panel_Voltages.voltage_samples_L2, CH_SAMPLE_NUMBER);
        arm_offset_f32(Panel_Voltages.voltage_samples_L3, -ADC_CHANNEL_OFFSET,
                       Panel_Voltages.voltage_samples_L3, CH_SAMPLE_NUMBER);
        //arm_offset_f32(Free_Channel, -ADC_CHANNEL_OFFSET, Free_Channel.ph_samples, CH_SAMPLE_NUMBER);

        // TODO: Debug option to visualize waves, remove this...
        arm_copy_f32(Panel_Voltages.voltage_samples_L1, wave3,
                     CH_SAMPLE_NUMBER);

        // Use CMSIS to multiply float, way faster....
        arm_scale_f32(Outlet_5.dif_samples, ADC_INPUT_SCALE,
                      Outlet_5.dif_samples, CH_SAMPLE_NUMBER);
        arm_scale_f32(Outlet_5.ph_samples, ADC_INPUT_SCALE, Outlet_5.ph_samples,
                      CH_SAMPLE_NUMBER);
        arm_scale_f32(Outlet_6.dif_samples, ADC_INPUT_SCALE,
                      Outlet_6.dif_samples, CH_SAMPLE_NUMBER);
        arm_scale_f32(Outlet_6.ph_samples, ADC_INPUT_SCALE, Outlet_6.ph_samples,
                      CH_SAMPLE_NUMBER);
        arm_scale_f32(Panel_Voltages.voltage_samples_L1, ADC_INPUT_SCALE,
                      Panel_Voltages.voltage_samples_L1, CH_SAMPLE_NUMBER);
        arm_scale_f32(Panel_Voltages.voltage_samples_L2, ADC_INPUT_SCALE,
                      Panel_Voltages.voltage_samples_L2, CH_SAMPLE_NUMBER);
        arm_scale_f32(Panel_Voltages.voltage_samples_L3, ADC_INPUT_SCALE,
                      Panel_Voltages.voltage_samples_L3, CH_SAMPLE_NUMBER);
        //arm_scale_f32(Free_Channel, ADC_INPUT_SCALE, Free_Channel, CH_SAMPLE_NUMBER);

        // Calculate RMS value
        arm_rms_f32(Outlet_5.dif_samples, CH_SAMPLE_NUMBER, &Outlet_5.dif_rms);
        arm_rms_f32(Outlet_5.ph_samples, CH_SAMPLE_NUMBER, &Outlet_5.ph_rms);
        arm_rms_f32(Outlet_6.dif_samples, CH_SAMPLE_NUMBER, &Outlet_6.dif_rms);
        arm_rms_f32(Outlet_6.ph_samples, CH_SAMPLE_NUMBER, &Outlet_6.ph_rms);
        arm_rms_f32(Panel_Voltages.voltage_samples_L1, CH_SAMPLE_NUMBER,
                    &Panel_Voltages.L1_rms);
        arm_rms_f32(Panel_Voltages.voltage_samples_L2, CH_SAMPLE_NUMBER,
                    &Panel_Voltages.L2_rms);
        arm_rms_f32(Panel_Voltages.voltage_samples_L3, CH_SAMPLE_NUMBER,
                    &Panel_Voltages.L3_rms);
        //arm_rms_f32(Free_Channel,CH_SAMPLE_NUMBER,&Free_Channel);

       // Outlet_1.id = 1;
//			if(tomada1.dif_rms >RMS_Limit)
//			{
//			    tomada1.id=2;
//			}

        // Ready to FFT
        Semaphore_post(data_proc3_fft_Sem);

        GPIO_write(DebugPin2, 0);
    }
}

void FFTCalc_Task()
{
    while (1)
    {
        Semaphore_pend(data_proc1_fft_Sem, BIOS_WAIT_FOREVER);
        GPIO_write(DebugPin1, 1);
        //arm_rfft_fast_instance_f32 s;

//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,Outlet_1.dif_samples,Outlet_1.dif_samples,0);
//		arm_cmplx_mag_f32(Outlet_1.dif_samples,Outlet_1.fft,CH_SAMPLE_NUMBER/2);//data_proc0_mag,CH_SAMPLE_NUMBER/2);

//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc1,data_proc1,0);
//		arm_cmplx_mag_f32(data_proc1,data_proc1_mag,CH_SAMPLE_NUMBER/2);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc2,data_proc2,0);
//		arm_cmplx_mag_f32(data_proc2,data_proc2_mag,CH_SAMPLE_NUMBER/2);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc3,data_proc3,0);
//		arm_cmplx_mag_f32(data_proc3,data_proc3_mag,CH_SAMPLE_NUMBER/2);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc4,data_proc4,0);
//		arm_cmplx_mag_f32(data_proc4,data_proc4_mag,CH_SAMPLE_NUMBER/2);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc5,data_proc5,0);
//		arm_cmplx_mag_f32(data_proc5,data_proc5_mag,CH_SAMPLE_NUMBER/2);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc6,data_proc6,0);
//		arm_cmplx_mag_f32(data_proc6,data_proc6_mag,CH_SAMPLE_NUMBER/2);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc7,data_proc7,0);
//		arm_cmplx_mag_f32(data_proc7,data_proc7_mag,CH_SAMPLE_NUMBER/2);
//
//		GPIO_write(DebugPin1,0);
        Semaphore_pend(data_proc3_fft_Sem, BIOS_WAIT_FOREVER);
//		GPIO_write(DebugPin2,1);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc8,data_proc8,0);
//		arm_cmplx_mag_f32(data_proc8,data_proc8_mag,CH_SAMPLE_NUMBER/2);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc9,data_proc9,0);
//		arm_cmplx_mag_f32(data_proc9,data_proc9_mag,CH_SAMPLE_NUMBER/2);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc10,data_proc10,0);
//		arm_cmplx_mag_f32(data_proc10,data_proc10_mag,CH_SAMPLE_NUMBER/2);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc11,data_proc11,0);
//		arm_cmplx_mag_f32(data_proc11,data_proc11_mag,CH_SAMPLE_NUMBER/2);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc12,data_proc12,0);
//		arm_cmplx_mag_f32(data_proc12,data_proc12_mag,CH_SAMPLE_NUMBER/2);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc13,data_proc13,0);
//		arm_cmplx_mag_f32(data_proc13,data_proc13_mag,CH_SAMPLE_NUMBER/2);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc14,data_proc14,0);
//		arm_cmplx_mag_f32(data_proc14,data_proc14_mag,CH_SAMPLE_NUMBER/2);
//
//		arm_rfft_fast_init_f32(&s,CH_SAMPLE_NUMBER);
//		arm_rfft_fast_f32(&s,data_proc15,data_proc15,0);
//		arm_cmplx_mag_f32(data_proc15,data_proc15_mag,CH_SAMPLE_NUMBER/2);

        Semaphore_post(fft_end_Sem);
        GPIO_write(DebugPin2, 0);

        if (uDMATransferCount > 2)
        {
            GPIO_write(DebugPin2, 0);
        }
    }
}


/*
 *  ======== echoFxn ========
 *  Task for this function is created statically. See the project's .cfg file.
 *  CRC = X^16+X^12+X^5+X^1
 *  https://www.lammertbies.nl/comm/info/crc-calculation.html
 *  Tested TAG: 12 34 56 78 00 00 00 00
 *  CRC = 63 FE
 *  TODO: Implement CRC check
 *  CRCConfigSet(CCM0_BASE, CRC_CFG_INIT_0 | CRC_CFG_TYPE_P1021 | CRC_CFG_SIZE_8BIT);
 */
Void Read_RFID(void)
{
    char input[19];
    UART_Handle uart;
    UART_Params uartParams;
    //const char echoPrompt[] = "\fEchoing characters:\r\n";

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 15625;
    uart = UART_open(Board_UART0, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
    }

    //UART_write(uart, echoPrompt, sizeof(echoPrompt));

    //Outlet_1.id=0;
    /* Loop forever echoing */
    while (1) {

        UART_read(uart, &input, 2);
        if((input[0]==0x50)&(input[1]==0xfe))
        {
        UART_read(uart, &input, 17);
        //if((input[0]==0x50)&(input[10]==0xfe))
        //{
            Outlet_1.id[0] = input[7];
            Outlet_1.id[1] = input[6];
            Outlet_1.id[2] = input[5];
            Outlet_1.id[3] = input[4];
            Outlet_1.id[4] = input[3];
            Outlet_1.id[5] = input[2];
            Outlet_1.id[6] = input[1];
            Outlet_1.id[7] = input[0];
        }
        else
        {
            Outlet_1.id[0] = 0;
            Outlet_1.id[1] = 0;
            Outlet_1.id[2] = 0;
            Outlet_1.id[3] = 0;
            Outlet_1.id[4] = 0;
            Outlet_1.id[5] = 0;
            Outlet_1.id[6] = 0;
            Outlet_1.id[7] = 0;
        }
        //}
        //UART_write(uart,"32", 2);
        SysCtlDelay(2000000);
    }
}

void RFID_SCIO(unsigned int index)
{
    //uint32_t data=0;
    //uint32_t i;
    //GPIO_disableInt(Board_RFID_SCIO);
    SysCtlDelay(2600); //  =~65us
//    for(i=0;i<8;i++)
//    {
//        data|=GPIO_read(RFIDPinSCIO)<<i;
//        SysCtlDelay(2500); //  =~65us
//    }
//    rfid=i;
//    SysCtlDelay(2600); //  =~65us
    //GPIO_enableInt(Board_RFID_SCIO);
}

void HeartBeat_Idle(void)
{
    GPIO_toggle(Board_LED1);
    SysCtlDelay(1000000);
}
