#include <arduino.h>
#include "OPL2.h"
#ifndef OPL2AB_LIB_H_
	#define OPL2AB_LIB_H_

	#define OPL2_BOARD_TYPE_ARDUINO      0
	#define OPL2_BOARD_TYPE_RASPBERRY_PI 1

	// !!! IMPORTANT !!!
	// In order to correctly compile the library for your platform be sure to set the correct BOARD_TYPE below.
	#define BOARD_TYPE OPL2_BOARD_TYPE_ARDUINO

	#if BOARD_TYPE == OPL2_BOARD_TYPE_ARDUINO
		#define PIN_LATCH 10
		#define PIN_ADDR   9
		#define PIN_RESET  8
	#else
		#define PIN_LATCH 3				// GPIO header pin 15
		#define PIN_ADDR  4				// GPIO header pin 16
		#define PIN_RESET 2				// GPIO header pin 13

		// SPI setup for WiringPi.
		#define SPI_SPEED   8000000
		#define SPI_CHANNEL 0
	#endif

	// Instrument data sources (Arduino only).
	#if BOARD_TYPE == OPL2_BOARD_TYPE_ARDUINO
		#define INSTRUMENT_DATA_PROGMEM true
		#define INSTRUMENT_DATA_SRAM false
	#endif

	#if BOARD_TYPE == OPL2_BOARD_TYPE_ARDUINO
		#include <Arduino.h>
	#else
		#include <stdint.h>
		#include <algorithm>
		typedef uint8_t byte;
		#define PROGMEM 
	#endif

	class OPL2AB:public OPL2 {
		public:
			OPL2AB();
			OPL2AB(byte reset, byte address, byte latch);
      void setupInterface();
			void hardReset();
			void write(byte reg, byte data);
		protected:
			template <typename T>
			T clampValue(T value, T min, T max);

			byte pinReset   = PIN_RESET;
			byte pinAddress = PIN_ADDR;
			byte pinLatch   = PIN_LATCH;
	};
#endif
