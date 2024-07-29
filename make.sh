#!/bin/bash

cmake\
 -DARM_NONE_EABI_TOOLCHAIN_PATH=/home/timod/bin/gcc-arm-none-eabi-10.3-2021.10\
 -DNRF5_SDK_PATH=/home/timod/bin/nRF5_SDK_15.3.0_59ac345\
 -DBUILD_DFU=1\
 -DBUILD_RESOURCES=1\
 -DENABLE_DFU=1
