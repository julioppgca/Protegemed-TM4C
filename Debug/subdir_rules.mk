################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
configPkg/linker.cmd: ../RTOS_protegemed.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"C:/ti/xdctools_3_32_00_06_core/xs" --xdcpath="C:/ti/tirtos_tivac_2_16_01_14/packages;C:/ti/tirtos_tivac_2_16_01_14/products/tidrivers_tivac_2_16_01_13/packages;C:/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31/packages;C:/ti/tirtos_tivac_2_16_01_14/products/ndk_2_25_00_09/packages;C:/ti/tirtos_tivac_2_16_01_14/products/uia_2_00_05_50/packages;C:/ti/tirtos_tivac_2_16_01_14/products/ns_1_11_00_10/packages;C:/ti/ccsv6/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.tiva:TM4C1294NCPDT -r release -c "C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS" --compileOptions "-mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --opt_for_speed=0 --include_path=\"C:/Users/skelter/Protegemed/Protegemed\" --include_path=\"C:/ti/tirtos_tivac_2_16_01_14/products/ndk_2_25_00_09/packages/ti/ndk/inc/bsd\" --include_path=\"C:/ti/CMSIS-SP-00300-r4p5-00rel0/CMSIS/Include\" --include_path=\"C:/ti/tirtos_tivac_2_16_01_14/products/TivaWare_C_Series-2.1.1.71b\" --include_path=\"C:/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31/packages/ti/sysbios/posix\" --include_path=\"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include\" -g --gcc --define=ccs=\"ccs\" --define=ARM_MATH_CM4 --define=__FPU_PRESENT=1 --define=PART_TM4C1294NCPDT --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi --ual  " "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/compiler.opt: | configPkg/linker.cmd
configPkg/: | configPkg/linker.cmd

main_protegemed.obj: ../main_protegemed.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --opt_for_speed=0 --include_path="C:/Users/skelter/Protegemed/Protegemed" --include_path="C:/ti/tirtos_tivac_2_16_01_14/products/ndk_2_25_00_09/packages/ti/ndk/inc/bsd" --include_path="C:/ti/CMSIS-SP-00300-r4p5-00rel0/CMSIS/Include" --include_path="C:/ti/tirtos_tivac_2_16_01_14/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31/packages/ti/sysbios/posix" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" -g --gcc --define=ccs="ccs" --define=ARM_MATH_CM4 --define=__FPU_PRESENT=1 --define=PART_TM4C1294NCPDT --define=ccs --define=TIVAWARE --diag_wrap=off --diag_warning=225 --diag_warning=255 --display_error_number --gen_func_subsections=on --abi=eabi --ual --preproc_with_compile --preproc_dependency="main_protegemed.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


