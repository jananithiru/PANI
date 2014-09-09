################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../bPlusTree/BPTPage.cpp \
../bPlusTree/BPlusTreeC.cpp \
../bPlusTree/BPlusTreeTest.cpp \
../bPlusTree/BPlusTreeUI.cpp 

OBJS += \
./bPlusTree/BPTPage.o \
./bPlusTree/BPlusTreeC.o \
./bPlusTree/BPlusTreeTest.o \
./bPlusTree/BPlusTreeUI.o 

CPP_DEPS += \
./bPlusTree/BPTPage.d \
./bPlusTree/BPlusTreeC.d \
./bPlusTree/BPlusTreeTest.d \
./bPlusTree/BPlusTreeUI.d 


# Each subdirectory must supply rules for building sources it contributes
bPlusTree/%.o: ../bPlusTree/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


