#______________________________________________________________________________
#│                                                                            |
#│ COPYRIGHT (C) 2021 Mihai Baneu                                             |
#│                                                                            |
#| Permission is hereby  granted,  free of charge,  to any person obtaining a |
#| copy of this software and associated documentation files (the "Software"), |
#| to deal in the Software without restriction,  including without limitation |
#| the rights to  use, copy, modify, merge, publish, distribute,  sublicense, |
#| and/or sell copies  of  the Software, and to permit  persons to  whom  the |
#| Software is furnished to do so, subject to the following conditions:       |
#|                                                                            |
#| The above  copyright notice  and this permission notice  shall be included |
#| in all copies or substantial portions of the Software.                     |
#|                                                                            |
#| THE SOFTWARE IS PROVIDED  "AS IS",  WITHOUT WARRANTY OF ANY KIND,  EXPRESS |
#| OR   IMPLIED,   INCLUDING   BUT   NOT   LIMITED   TO   THE  WARRANTIES  OF |
#| MERCHANTABILITY,  FITNESS FOR  A  PARTICULAR  PURPOSE AND NONINFRINGEMENT. |
#| IN NO  EVENT SHALL  THE AUTHORS  OR  COPYRIGHT  HOLDERS  BE LIABLE FOR ANY |
#| CLAIM, DAMAGES OR OTHER LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT |
#| OR OTHERWISE, ARISING FROM,  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR  |
#| THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                 |
#|____________________________________________________________________________|
#|                                                                            |
#|  Author: Mihai Baneu                           Last modified: 03.Jan.2021  |
#|                                                                            |
#|____________________________________________________________________________|

CONFIG_MCU                  = STM32F411CE
CONFIG_OPENOCDDIR 			= ~/work/tools/openocd/src
CONFIG_OPENOCDCONFIGDIR		= ~/work/tools/openocd/tcl
CONFIG_OPENOCD_INTERFACE	= interface/stlink-v3.cfg
CONFIG_OPENOCD_BOARD		= board/stm32f411xx.cfg

.PHONY: all build clean

MAKECMDGOALS ?= all
all: build

config:
	/usr/bin/qbs config-ui
	
build: 
	/usr/bin/qbs build -d build -f source/project.qbs --jobs 16 config:$(CONFIG_MCU) qbs.installRoot:bin qbs.targetPlatform:$(CONFIG_MCU)

build-product: 
	/usr/bin/qbs build -d build/$(PRODUCT) -f source/$(PRODUCT)/$(PRODUCT).qbs --jobs 1 config:$(CONFIG_MCU) qbs.installRoot:bin qbs.targetPlatform:$(CONFIG_MCU)

build-with-commands:
	/usr/bin/qbs build -d build -f source/project.qbs --jobs 16 config:$(CONFIG_MCU) --command-echo-mode command-line qbs.installRoot:bin qbs.targetPlatform:$(CONFIG_MCU)

clean:
	/usr/bin/qbs clean -d build config:$(CONFIG_MCU)

debug:
	$(CONFIG_OPENOCDDIR)/openocd -s $(CONFIG_OPENOCDCONFIGDIR) -f $(CONFIG_OPENOCD_INTERFACE) -f $(CONFIG_OPENOCD_BOARD)

reset:
	$(CONFIG_OPENOCDDIR)/openocd -s $(CONFIG_OPENOCDCONFIGDIR) -f $(CONFIG_OPENOCD_INTERFACE) -f $(CONFIG_OPENOCD_BOARD) -c "init; reset; exit"

flash: all
	$(CONFIG_OPENOCDDIR)/openocd -s $(CONFIG_OPENOCDCONFIGDIR) -f $(CONFIG_OPENOCD_INTERFACE) -f $(CONFIG_OPENOCD_BOARD) -c "program bin/application.bin 0x08000000 verify reset exit"

flash-banks:
	$(CONFIG_OPENOCDDIR)/openocd -s $(CONFIG_OPENOCDCONFIGDIR) -f $(CONFIG_OPENOCD_INTERFACE) -f $(CONFIG_OPENOCD_BOARD) -c "init; echo [flash banks]; exit"

flash-list:
	$(CONFIG_OPENOCDDIR)/openocd -s $(CONFIG_OPENOCDCONFIGDIR) -f $(CONFIG_OPENOCD_INTERFACE) -f $(CONFIG_OPENOCD_BOARD) -c "init; echo [flash list]; exit"

flash-info:
	$(CONFIG_OPENOCDDIR)/openocd -s $(CONFIG_OPENOCDCONFIGDIR) -f $(CONFIG_OPENOCD_INTERFACE) -f $(CONFIG_OPENOCD_BOARD) -c "init; echo [flash info $(bank)]; exit"

flash-read:
	$(CONFIG_OPENOCDDIR)/openocd -s $(CONFIG_OPENOCDCONFIGDIR) -f $(CONFIG_OPENOCD_INTERFACE) -f $(CONFIG_OPENOCD_BOARD) -c "init; echo [flash read_bank $(bank) bin/flash_$(bank).hex]; exit"

stflash:
	~/work/tools/stlink/build/Release/bin/st-flash --reset write bin/application.bin 0x08000000

monitor:
	screen /dev/ttyACM0 115200

list-usb:
	./scripts/list-usb.sh
