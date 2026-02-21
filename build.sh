#!/bin/bash

source .venv/bin/activate

mkdir build
cd build

cmake ..\
 -DARM_NONE_EABI_TOOLCHAIN_PATH=$HOME/bin/gcc-arm-none-eabi-10.3-2021.10\
 -DNRF5_SDK_PATH=$HOME/bin/nRF5_SDK_15.3.0_59ac345\
 -DBUILD_DFU=1\
 -DBUILD_RESOURCES=1\
 -DCMAKE_BUILD_TYPE=Release\
 -DENABLE_DFU=1

make -j4 pinetime-mcuboot-app

