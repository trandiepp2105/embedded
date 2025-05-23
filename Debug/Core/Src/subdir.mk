################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/LTM022A69B.c \
../Core/Src/font.c \
../Core/Src/fram_i2c.c \
../Core/Src/main.c \
../Core/Src/manual_lcd.c \
../Core/Src/manual_touch.c \
../Core/Src/rtc_utils.c \
../Core/Src/sd_card_manager.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/task2.c 

OBJS += \
./Core/Src/LTM022A69B.o \
./Core/Src/font.o \
./Core/Src/fram_i2c.o \
./Core/Src/main.o \
./Core/Src/manual_lcd.o \
./Core/Src/manual_touch.o \
./Core/Src/rtc_utils.o \
./Core/Src/sd_card_manager.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/task2.o 

C_DEPS += \
./Core/Src/LTM022A69B.d \
./Core/Src/font.d \
./Core/Src/fram_i2c.d \
./Core/Src/main.d \
./Core/Src/manual_lcd.d \
./Core/Src/manual_touch.d \
./Core/Src/rtc_utils.d \
./Core/Src/sd_card_manager.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/task2.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fcommon -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/LTM022A69B.cyclo ./Core/Src/LTM022A69B.d ./Core/Src/LTM022A69B.o ./Core/Src/LTM022A69B.su ./Core/Src/font.cyclo ./Core/Src/font.d ./Core/Src/font.o ./Core/Src/font.su ./Core/Src/fram_i2c.cyclo ./Core/Src/fram_i2c.d ./Core/Src/fram_i2c.o ./Core/Src/fram_i2c.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/manual_lcd.cyclo ./Core/Src/manual_lcd.d ./Core/Src/manual_lcd.o ./Core/Src/manual_lcd.su ./Core/Src/manual_touch.cyclo ./Core/Src/manual_touch.d ./Core/Src/manual_touch.o ./Core/Src/manual_touch.su ./Core/Src/rtc_utils.cyclo ./Core/Src/rtc_utils.d ./Core/Src/rtc_utils.o ./Core/Src/rtc_utils.su ./Core/Src/sd_card_manager.cyclo ./Core/Src/sd_card_manager.d ./Core/Src/sd_card_manager.o ./Core/Src/sd_card_manager.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/task2.cyclo ./Core/Src/task2.d ./Core/Src/task2.o ./Core/Src/task2.su

.PHONY: clean-Core-2f-Src

