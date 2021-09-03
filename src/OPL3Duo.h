#include <arduino.h>
#include "OPL3.h"

#ifndef OPL3DUO_LIB_H_
	#define OPL3DUO_LIB_H_

	#define NUM_4OP_CHANNELS_PER_UNIT 6
	#define OPL3DUO_NUM_2OP_CHANNELS 36
	#define OPL3DUO_NUM_4OP_CHANNELS 12

	// !!! IMPORTANT !!!
	// In order to correctly compile the library for your platform be sure to set the correct BOARD_TYPE below.
	#define BOARD_TYPE OPL2_BOARD_TYPE_ARDUINO

	#if BOARD_TYPE == OPL2_BOARD_TYPE_ARDUINO
		#define PIN_BANK   7
		#define PIN_LATCH 10
		#define PIN_ADDR   9
		#define PIN_RESET  8
    #define PIN_UNIT   6
	#else
		#define PIN_LATCH 3				// GPIO header pin 15
		#define PIN_ADDR  4				// GPIO header pin 16
		#define PIN_RESET 2				// GPIO header pin 13
		#define PIN_BANK 5				// GPIO header pin 18
    #define PIN_UNIT 

		// SPI setup for WiringPi.
		#define SPI_SPEED   8000000
		#define SPI_CHANNEL 0
	#endif

	class OPL3Duo: public OPL3 {
		public:
			OPL3Duo();
			OPL3Duo(byte a2, byte a1, byte a0, byte latch, byte reset);
			void reset();
      void hardReset();
      void setupInterface();
			void createShadowRegisters();

			byte getChipRegister(byte synthUnit, short reg);
			void setChipRegister(byte synthUnit, short reg, byte value);
			void setChannelRegister(byte baseRegister, byte channel, byte value);
			void setOperatorRegister(byte baseRegister, byte channel, byte op, byte value);
			void write(byte bank, byte reg, byte value);
      void write(byte reg, byte value);

			byte getNumChannels();
			byte getNum4OPChannels();
			byte get4OPControlChannel(byte channel4OP, byte index2OP = 0);

			bool isOPL3Enabled();
			bool isOPL3Enabled(byte synthUnit);
			void setOPL3Enabled(bool enable);
			void setOPL3Enabled(byte synthUnit, bool enable);
			bool is4OPChannelEnabled(byte channel4OP);
			void set4OPChannelEnabled(byte channel4OP, bool enable);
			void setAll4OPChannelsEnabled(bool enable);
			void setAll4OPChannelsEnabled(byte synthUnit, bool enable);
		protected:
			byte pinUnit = PIN_UNIT;
			byte pinBank = PIN_BANK;
      byte pinAddress = PIN_ADDR;
      byte pinLatch   = PIN_LATCH;
      byte pinReset = PIN_RESET;

			byte numChannels = OPL3DUO_NUM_2OP_CHANNELS;
			byte num4OPChannels = OPL3DUO_NUM_4OP_CHANNELS;
	};
#endif
