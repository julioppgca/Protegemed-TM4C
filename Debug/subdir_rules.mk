################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
build-186446209: ../RTOS_protegemed.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"/Users/juliosantos/ti/tirex-content/xdctools_3_32_00_06_core/xs" --xdcpath="/Users/juliosantos/ti/tirex-content/tirtos_tivac_2_16_00_08/packages;/Users/juliosantos/ti/tirex-content/tirtos_tivac_2_16_00_08/products/tidrivers_tivac_2_16_00_08/packages;/Users/juliosantos/ti/tirex-content/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages;/Users/juliosantos/ti/tirex-content/tirtos_tivac_2_16_00_08/products/ndk_2_25_00_09/packages;/Users/juliosantos/ti/tirex-content/tirtos_tivac_2_16_00_08/products/uia_2_00_05_50/packages;/Users/juliosantos/ti/tirex-content/tirtos_tivac_2_16_00_08/products/ns_1_11_00_10/packages;/Applications/ti/ccsv7/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.tiva:TM4C1294NCPDT -r release -c "/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS" --compileOptions "-mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --opt_for_speed=0 --include_path=\"/Users/juliosantos/Protegemed-TM4C\" --include_path=\"/Users/juliosantos/ti/tirex-content/tirtos_tivac_2_16_00_08/products/ndk_2_25_00_09/packages/ti/ndk/inc/bsd\" --include_path=\"/Applications/ti/CMSIS-SP-00300-r4p5-00rel0/CMSIS/Include\" --include_path=\"/Users/juliosantos/ti/tirex-content/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b\" --include_path=\"/Users/juliosantos/ti/tirex-content/tirtos_tivac_2_16_00_08/products/bios_6_45_02_31/packages/ti/sysbios/posix\" --include_path=\"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include\" --define=ccs=\"ccs\" --define=ARM_MATH_CM4 --define=__FPU_PRESENT=1 --define=PART_TM4C1294NCPDT --define=ccs --define=TIVAWARE -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --ual  " "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/linker.cmd: build-186446209
configPkg/compiler.opt: build-186446209
configPkg/: build-186446209

main_protegemed.obj: ../main_protegemed.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --opt_for_speed=0 --include_path="/Users/juliosantos/Protegemed-TM4C" --include_path="/Users/juliosantos/ti/tirex-content/tirtos_tivac_2_16_00_08/products/ndk_2_25_00_09/packages/ti/ndk/inc/bsd" --include_path="/Applications/ti/CMSIS-SP-00300-r4p5-00rel0/CMSIS/Include" --include_path="/Users/juliosantos/ti/tirex-content/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b" --include_path="/Users/juliosantos/ti/tirex-content/tirtos_tivac_2_16_00_08/products/bios_6_45_02_31/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --define=ccs="ccs" --define=ARM_MATH_CM4 --define=__FPU_PRESENT=1 --define=PART_TM4C1294NCPDT --define=ccs --define=TIVAWARE -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --ual --preproc_with_compile --preproc_dependency="main_protegemed.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


