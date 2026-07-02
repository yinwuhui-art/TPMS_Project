################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-636924177: ../basic_ble.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"E:/ti/sysconfig_1.26.3/sysconfig_cli.bat" --script "E:/CCS12.8.1/workspace_v12/basic_ble_LP_EM_CC2745R10_Q1_freertos_ticlang/basic_ble.syscfg" -o "syscfg" -s "E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/.metadata/product.json" --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_ble_config.h: build-636924177 ../basic_ble.syscfg
syscfg/ti_ble_config.c: build-636924177
syscfg/ti_ble_oad_linker.inc: build-636924177
syscfg/ti_ble_oad_postbuild.cfg: build-636924177
syscfg/ti_ble_oad_prebuild.cfg: build-636924177
syscfg/ti_devices_config.c: build-636924177
syscfg/ti_secure_boot_properties.json: build-636924177
syscfg/ti_secure_boot_properties.h: build-636924177
syscfg/ti_radio_config.c: build-636924177
syscfg/ti_radio_config.h: build-636924177
syscfg/ti_drivers_config.c: build-636924177
syscfg/ti_drivers_config.h: build-636924177
syscfg/ti_utils_build_linker.cmd.genlibs: build-636924177
syscfg/ti_utils_build_linker.cmd.genmap: build-636924177
syscfg/ti_utils_build_compiler.opt: build-636924177
syscfg/syscfg_c.rov.xs: build-636924177
syscfg/FreeRTOSConfig.h: build-636924177
syscfg/ti_freertos_config.c: build-636924177
syscfg/ti_freertos_portable_config.c: build-636924177
syscfg: build-636924177

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"E:/CCS12.8.1/ccs/tools/compiler/ti-cgt-armllvm_3.2.2.LTS/bin/tiarmclang.exe" -c @"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti/ble/stack_util/config/build_components.opt" @"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti/ble/stack_util/config/factory_config.opt"  -mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mlittle-endian -mthumb -Oz -I"E:/CCS12.8.1/workspace_v12/basic_ble_LP_EM_CC2745R10_Q1_freertos_ticlang" -I"E:/CCS12.8.1/workspace_v12/basic_ble_LP_EM_CC2745R10_Q1_freertos_ticlang/Debug" -I"E:/CCS12.8.1/workspace_v12/basic_ble_LP_EM_CC2745R10_Q1_freertos_ticlang/app" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti/common/cc26xx" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/third_party/mbedtls/include" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/third_party/mbedtls/ti" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/third_party/mcuboot/boot/bootutil/include" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/third_party/mcuboot/boot/ti/source/mcuboot_app" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti/common/cc26xx/flash_interface" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti/drivers/rcl" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti/posix/ticlang" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/kernel/freertos" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/third_party/freertos/include" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/third_party/freertos/portable/GCC/ARM_CM33_NTZ/non_secure" -DICALL_NO_APP_EVENTS -DMBEDTLS_CONFIG_FILE="configs/config-hsm.h" -DMBEDTLS_PSA_CRYPTO_CONFIG_FILE="configs/config-psa-crypto-hsm.h" -DNVOCMP_NVPAGES=6 -DNVOCMP_POSIX_MUTEX -DNVOCMP_NWSAMEITEM=1 -DFREERTOS -DUSE_HSM -g -Wunused-function -Wall -ffunction-sections -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"E:/CCS12.8.1/workspace_v12/basic_ble_LP_EM_CC2745R10_Q1_freertos_ticlang/Debug/syscfg" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"E:/CCS12.8.1/ccs/tools/compiler/ti-cgt-armllvm_3.2.2.LTS/bin/tiarmclang.exe" -c @"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti/ble/stack_util/config/build_components.opt" @"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti/ble/stack_util/config/factory_config.opt"  -mcpu=cortex-m33 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mlittle-endian -mthumb -Oz -I"E:/CCS12.8.1/workspace_v12/basic_ble_LP_EM_CC2745R10_Q1_freertos_ticlang" -I"E:/CCS12.8.1/workspace_v12/basic_ble_LP_EM_CC2745R10_Q1_freertos_ticlang/Debug" -I"E:/CCS12.8.1/workspace_v12/basic_ble_LP_EM_CC2745R10_Q1_freertos_ticlang/app" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti/common/cc26xx" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/third_party/mbedtls/include" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/third_party/mbedtls/ti" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/third_party/mcuboot/boot/bootutil/include" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/third_party/mcuboot/boot/ti/source/mcuboot_app" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti/common/cc26xx/flash_interface" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti/drivers/rcl" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/ti/posix/ticlang" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/kernel/freertos" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/third_party/freertos/include" -I"E:/ti/simplelink_lowpower_f3_sdk_9_20_00_81/source/third_party/freertos/portable/GCC/ARM_CM33_NTZ/non_secure" -DICALL_NO_APP_EVENTS -DMBEDTLS_CONFIG_FILE="configs/config-hsm.h" -DMBEDTLS_PSA_CRYPTO_CONFIG_FILE="configs/config-psa-crypto-hsm.h" -DNVOCMP_NVPAGES=6 -DNVOCMP_POSIX_MUTEX -DNVOCMP_NWSAMEITEM=1 -DFREERTOS -DUSE_HSM -g -Wunused-function -Wall -ffunction-sections -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"E:/CCS12.8.1/workspace_v12/basic_ble_LP_EM_CC2745R10_Q1_freertos_ticlang/Debug/syscfg" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


