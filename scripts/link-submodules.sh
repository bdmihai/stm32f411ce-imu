#!/bin/sh

git submodule add git@github.com:bdmihai/stm32-doc doc/stm32
git submodule add git@github.com:bdmihai/stm32-qbs qbs/modules/stm32
git submodule add git@github.com:bdmihai/stm32-startup source/startup
git submodule add git@github.com:bdmihai/stm32-linker source/linker
git submodule add git@github.com:bdmihai/stm32-hal source/hal
git submodule add git@github.com:bdmihai/stm32-cmsis source/cmsis
git submodule add git@github.com:bdmihai/stm32-freertos source/freertos
git submodule add git@github.com:bdmihai/stm32-rencoder source/rencoder
git submodule add git@github.com:bdmihai/stm32-uprintf source/uprintf
git submodule add git@github.com:bdmihai/stm32-st7735 source/st7735
git submodule add git@github.com:bdmihai/stm32-mpu6050.git source/mpu6050
