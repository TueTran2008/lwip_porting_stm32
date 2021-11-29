################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Modules/UserUart/protocol/Slip/User_Slip.c 

OBJS += \
./Drivers/Modules/UserUart/protocol/Slip/User_Slip.o 

C_DEPS += \
./Drivers/Modules/UserUart/protocol/Slip/User_Slip.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Modules/UserUart/protocol/Slip/%.o: ../Drivers/Modules/UserUart/protocol/Slip/%.c Drivers/Modules/UserUart/protocol/Slip/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/App_Test -I../Drivers/Mid/base -I../Drivers/LoRa_Drivers/Modules -I../Drivers/LoRa_Drivers/Modules/Config -I../Drivers/LoRa_Drivers/Modules/DataLinkLayer -I../Drivers/LoRa_Drivers/Modules/NetworkLayer -I../Drivers/LoRa_Drivers/Modules/PhysicLayer -I../Drivers/LoRa_Drivers/Devices/SX126X -I../Drivers/LoRa_Drivers/Devices/SX126X/delay -I../Drivers/LoRa_Drivers/Devices/SX126X/inc -I../Drivers/LoRa_Drivers/Devices/SX126X/src -I../Drivers/LoRa_Drivers/Aloha_Protocol -I../Drivers/lwip-2.1.2/src -I../Drivers/lwip-2.1.2/src/api -I../Drivers/lwip-2.1.2/src/apps -I../Drivers/lwip-2.1.2/src/core -I../Drivers/lwip-2.1.2/src/include -I../Drivers/lwip-2.1.2/src/netif -I../Drivers/lwip-2.1.2/src/include/compat -I../Drivers/lwip-2.1.2/src/include/lwip -I../Drivers/lwip-2.1.2/src/include/netif -I../Drivers/lwip-2.1.2/src/include/lwip/apps -I../Drivers/lwip-2.1.2/src/include/lwip/priv -I../Drivers/lwip-2.1.2/src/include/lwip/prot -I../Drivers/lwip-2.1.2/test -I../Drivers/lwip-2.1.2/test/fuzz -I../Drivers/lwip-2.1.2/stm32 -I../Drivers/lwip-2.1.2/stm32/arch -I../Drivers/lwip-2.1.2/stm32/SEGGER_RTT_V698b/RTT -I../Drivers/lwip-2.1.2/stm32/SEGGER_RTT_V698b/Syscalls -I../Drivers/lwip-2.1.2/stm32/app_debug -I../Drivers/lwip-2.1.2/src/apps/http/makefsdata -I../Drivers/Modules/Button -I../Drivers/lwip-2.1.2/gsm -I../Drivers/lwip-2.1.2/src/include/netif/ppp -I../Drivers/Modules/UserUart -I../Drivers/Modules/UserUart/cfg -I../Drivers/Modules/UserUart/protocol -I../Drivers/Modules/UserUart/ip/usart -I../Drivers/lwip-2.1.2/mqtt_client -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

