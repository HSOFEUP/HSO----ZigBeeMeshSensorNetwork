################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../cpptophp.cpp \
../main.cpp \
../serial_util1.cpp \
../xbee_api_linux.cpp 

OBJS += \
./cpptophp.o \
./main.o \
./serial_util1.o \
./xbee_api_linux.o 

CPP_DEPS += \
./cpptophp.d \
./main.d \
./serial_util1.d \
./xbee_api_linux.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


