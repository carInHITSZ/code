################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../code/Attitude.c \
../code/C_H.c \
../code/Fit.c \
../code/PID.c \
../code/QuaternionEKF.c \
../code/TLD.c \
../code/encoder.c \
../code/initial.c \
../code/ips200.c \
../code/kalman\ filter.c \
../code/matrix.c \
../code/menu_simp.c \
../code/zf_device_dot_matrix_screen.c \
../code/zf_device_tld7002.c 

COMPILED_SRCS += \
./code/Attitude.src \
./code/C_H.src \
./code/Fit.src \
./code/PID.src \
./code/QuaternionEKF.src \
./code/TLD.src \
./code/encoder.src \
./code/initial.src \
./code/ips200.src \
./code/kalman\ filter.src \
./code/matrix.src \
./code/menu_simp.src \
./code/zf_device_dot_matrix_screen.src \
./code/zf_device_tld7002.src 

C_DEPS += \
./code/Attitude.d \
./code/C_H.d \
./code/Fit.d \
./code/PID.d \
./code/QuaternionEKF.d \
./code/TLD.d \
./code/encoder.d \
./code/initial.d \
./code/ips200.d \
./code/kalman\ filter.d \
./code/matrix.d \
./code/menu_simp.d \
./code/zf_device_dot_matrix_screen.d \
./code/zf_device_tld7002.d 

OBJS += \
./code/Attitude.o \
./code/C_H.o \
./code/Fit.o \
./code/PID.o \
./code/QuaternionEKF.o \
./code/TLD.o \
./code/encoder.o \
./code/initial.o \
./code/ips200.o \
./code/kalman\ filter.o \
./code/matrix.o \
./code/menu_simp.o \
./code/zf_device_dot_matrix_screen.o \
./code/zf_device_tld7002.o 


# Each subdirectory must supply rules for building sources it contributes
code/%.src: ../code/%.c code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -cs --dep-file="$(basename $@).d" --misrac-version=2004 "-fC:/Users/HUAWEI/Desktop/new/TLD7002_TC264_Demo/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<" && \
	if [ -f "$(basename $@).d" ]; then sed.exe -r  -e 's/\b(.+\.o)\b/code\/\1/g' -e 's/\\/\//g' -e 's/\/\//\//g' -e 's/"//g' -e 's/([a-zA-Z]:\/)/\L\1/g' -e 's/\d32:/@TARGET_DELIMITER@/g; s/\\\d32/@ESCAPED_SPACE@/g; s/\d32/\\\d32/g; s/@ESCAPED_SPACE@/\\\d32/g; s/@TARGET_DELIMITER@/\d32:/g' "$(basename $@).d" > "$(basename $@).d_sed" && cp "$(basename $@).d_sed" "$(basename $@).d" && rm -f "$(basename $@).d_sed" 2>/dev/null; else echo 'No dependency file to process';fi
	@echo 'Finished building: $<'
	@echo ' '

code/%.o: ./code/%.src code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

code/kalman\ filter.src: ../code/kalman\ filter.c code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -cs --dep-file="$(basename $@).d" --misrac-version=2004 "-fC:/Users/HUAWEI/Desktop/new/TLD7002_TC264_Demo/Debug/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<" && \
	if [ -f "$(basename $@).d" ]; then sed.exe -r  -e 's/\b(.+\.o)\b/code\/\1/g' -e 's/\\/\//g' -e 's/\/\//\//g' -e 's/"//g' -e 's/([a-zA-Z]:\/)/\L\1/g' -e 's/\d32:/@TARGET_DELIMITER@/g; s/\\\d32/@ESCAPED_SPACE@/g; s/\d32/\\\d32/g; s/@ESCAPED_SPACE@/\\\d32/g; s/@TARGET_DELIMITER@/\d32:/g' "$(basename $@).d" > "$(basename $@).d_sed" && cp "$(basename $@).d_sed" "$(basename $@).d" && rm -f "$(basename $@).d_sed" 2>/dev/null; else echo 'No dependency file to process';fi
	@echo 'Finished building: $<'
	@echo ' '

code/kalman\ filter.o: ./code/kalman\ filter.src code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-code

clean-code:
	-$(RM) ./code/Attitude.d ./code/Attitude.o ./code/Attitude.src ./code/C_H.d ./code/C_H.o ./code/C_H.src ./code/Fit.d ./code/Fit.o ./code/Fit.src ./code/PID.d ./code/PID.o ./code/PID.src ./code/QuaternionEKF.d ./code/QuaternionEKF.o ./code/QuaternionEKF.src ./code/TLD.d ./code/TLD.o ./code/TLD.src ./code/encoder.d ./code/encoder.o ./code/encoder.src ./code/initial.d ./code/initial.o ./code/initial.src ./code/ips200.d ./code/ips200.o ./code/ips200.src ./code/kalman\ filter.d ./code/kalman\ filter.o ./code/kalman\ filter.src ./code/matrix.d ./code/matrix.o ./code/matrix.src ./code/menu_simp.d ./code/menu_simp.o ./code/menu_simp.src ./code/zf_device_dot_matrix_screen.d ./code/zf_device_dot_matrix_screen.o ./code/zf_device_dot_matrix_screen.src ./code/zf_device_tld7002.d ./code/zf_device_tld7002.o ./code/zf_device_tld7002.src

.PHONY: clean-code

