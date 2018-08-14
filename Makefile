
# NICE_ERRORS=1

### BOARD_TAG & BOARD_SUB
### For Arduino IDE 1.0.x
### Only BOARD_TAG is needed. It must be set to the board you are currently using. (i.e uno, mega2560, etc.)
# BOARD_TAG         = mega2560
### For Arduino IDE 1.6.x
### Both BOARD_TAG and BOARD_SUB are needed. They must be set to the board you are currently using. (i.e BOARD_TAG = uno, mega, etc. & BOARD_SUB = atmega2560, etc.)
### Note: for the Arduino Uno, only BOARD_TAG is mandatory and BOARD_SUB can be equal to anything
BOARD_TAG         = mega
BOARD_SUB         = atmega2560

MCU = atmega2560

### MONITOR_BAUDRATE
### It must be set to Serial baudrate value you are using.
MONITOR_BAUDRATE  = 9600
### 115200



ifdef NICE_ERRORS
	NICE_ERRORS_COMPILE_ONLY = 1

	CC_NAME      = clang
	CXX_NAME     = clang++

	CFLAGS_STD        = -std=c11
	CXXFLAGS_STD      = -std=c++11

	CXXALTS = -DNICE_ERRORS

	EXTRA_INCLUDES = -I/usr/lib/avr/include -I/usr/share/arduino/hardware/arduino/cores/arduino -I/usr/share/arduino/hardware/arduino/variants/mega    -I/usr/share/arduino/libraries/Servo    -I/usr/share/arduino/libraries/Wire -I/usr/share/arduino/libraries/Wire -I/usr/share/arduino/libraries/Wire/utility

else
	CC_NAME      = avr-gcc
	CXX_NAME     = avr-g++

	CFLAGS_STD        = -std=gnu11
	CXXFLAGS_STD      = -std=gnu++11

endif


### PROJECT_DIR
### This is the path to where you have created/cloned your project
PROJECT_DIR       = $(shell expr `pwd`)

### AVR_GCC_VERSION
### Check if the version is equal or higher than 4.9
AVR_GCC_VERSION  := $(shell expr `avr-gcc -dumpversion | cut -f1` \>= 4.9)

### ARDMK_DIR
### Path to the Arduino-Makefile directory.
ARDMK_DIR         = ../Bare-Arduino-Project/Arduino-Makefile

### ARDUINO_DIR
### Path to the Arduino application and ressources directory.
ARDUINO_DIR       = /usr/share/arduino

### USER_LIB_PATH
### Path to where the your project's libraries are stored.
USER_LIB_PATH     :=  $(realpath $(PROJECT_DIR)/deps)

### AVR_TOOLS_DIR
### Path to the AVR tools directory such as avr-gcc, avr-g++, etc.
AVR_TOOLS_DIR     = /usr

### AVRDDUDE
### Path to avrdude directory.
AVRDDUDE          = /usr/bin/avrdude
AVRDUDE2 		  = /usr/share/arduino/hardware/tools/avrdude

### CPPFLAGS
### Flags you might want to set for debugging purpose. Comment to stop.
CXXFLAGS_WARNS    = -pedantic -Wall -Wextra


CXXFLAGS = $(CXXFLAGS_WARNS) $(EXTRA_INCLUDES)
CXXFLAGS += $(CXXALTS)
CXXFLAGS += $(CXXFLAGS_STD)

# *TODO* this should be completely fucking automatic!
CXXFLAGS += -I/home/oscar/ xxx /libraries/OneWire
CXXFLAGS += -I/home/oscar/ xxx /libraries/DallasTemperature/
# CXXFLAGS += -I/home/oscar/sketchbook/libraries/RTClib/

CFLAGS = $(CFLAGS_STD)

### If avr-gcc -v is higher than 4.9, activate coloring of the output
ifeq "$(AVR_GCC_VERSION)" "1"
    CXXFLAGS += -fdiagnostics-color
endif

### MONITOR_PORT
### The port your board is connected to. Using an '*' tries all the ports and finds the right one.
MONITOR_PORT      = /dev/ttyACM0

### don't touch this
CURRENT_DIR       = $(shell basename $(CURDIR))

### OBJDIR
### This is were you put the binaries you just compile using 'make'
OBJDIR            = $(PROJECT_DIR)/build/$(CURRENT_DIR)/$(BOARD_TAG)

### path to Arduino.mk, inside the ARDMK_DIR, don't touch.
include $(ARDMK_DIR)/Arduino.mk


upload:
	# *TODO* this uses hardcoded paths badly
	$(AVRDUDE2) \
	-C/usr/share/arduino/hardware/tools/avrdude.conf \
	-v -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 \
	-D -U flash:w:/home/oscar/3-p/PATH_TO_BUILD_DONE_BINARY.hex:i

log:
	stty -F $MONITOR_PORT $MONITOR_BAUDRATE raw \
	-clocal -echo icrnl
	cat $MONITOR_PORT

