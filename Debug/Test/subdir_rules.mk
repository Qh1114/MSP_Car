################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Test/%.o: ../Test/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/TI/ccs2041/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/Qh/Desktop/ccs/MSP_Car" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/Debug" -I"C:/TI/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"C:/TI/mspm0_sdk_2_10_00_04/source" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/Middle/Fusion" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/Middle" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/Test" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/HardWare/vl53l1x/platform" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/HardWare/vl53l1x/core" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/HardWare" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/APP" -gdwarf-3 -Wall -MMD -MP -MF"Test/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

Test/Uart_Test.o: ../Test/Uart_Test.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Arm Compiler - building file: "$<"'
	"D:/TI/ccs2041/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/Qh/Desktop/ccs/MSP_Car" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/Debug" -I"C:/TI/mspm0_sdk_2_10_00_04/source/third_party/CMSIS/Core/Include" -I"C:/TI/mspm0_sdk_2_10_00_04/source" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/Middle/Fusion" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/Middle" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/Test" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/HardWare/vl53l1x/platform" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/HardWare/vl53l1x/core" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/HardWare" -I"C:/Users/Qh/Desktop/ccs/MSP_Car/APP" -gdwarf-3 -Wall -MMD -MP -MF"Test/$(basename $(<F)).d_raw" -MT"Test/$(basename\ $(<F)).o"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


