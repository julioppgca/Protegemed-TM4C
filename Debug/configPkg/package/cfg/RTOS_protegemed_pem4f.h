/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-B06
 */

#include <xdc/std.h>

#include <ti/sysbios/family/arm/m3/Hwi.h>
extern const ti_sysbios_family_arm_m3_Hwi_Handle ADC_Seq0_Handle;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle data_proc1_Sem;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle RMSCalc_Handle;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle data_proc1_fft_Sem;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle FFTCalc_Handle;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle data_proc2_Sem;

#include <ti/sysbios/family/arm/m3/Hwi.h>
extern const ti_sysbios_family_arm_m3_Hwi_Handle ADC_Seq1_Handle;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle data_proc3_Sem;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle data_proc4_Sem;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle data_proc3_fft_Sem;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle fft_end_Sem;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle InitSamples_Handle;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle Read_RFID_Handle;

extern int xdc_runtime_Startup__EXECFXN__C;

extern int xdc_runtime_Startup__RESETFXN__C;

