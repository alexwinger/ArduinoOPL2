#include <arduino.h>
#include <OPL3.h>

#ifndef OPL3DIRECT_LIB_H_
	#define OPL3DIRECT_LIB_H_
	// !!! IMPORTANT !!!
	// In order to correctly compile the library for your platform be sure to set the correct BOARD_TYPE below.
	#define BOARD_TYPE OPL2_BOARD_TYPE_ARDUINO

		#define PIN_A1     A0
		#define PIN_A0     A1
		#define PIN_RESET  16
		#define PIN_WR     15
		#define PIN_D0     14
		#define PIN_D1     10 
		#define PIN_D2      9
		#define PIN_D3      8
		#define PIN_D4      7
		#define PIN_D5      6
		#define PIN_D6      5
		#define PIN_D7      4

	class OPL3Direct: public OPL3 {
		public:
			OPL3Direct();
			OPL3Direct(byte a1, byte a0,byte wr, byte reset, byte d0,byte d1,byte d2, byte d3, byte d4, byte d5,byte d6,byte d7);
			void hardReset();
			void setupInterface();
			void write(byte bank, byte reg, byte value);
			void write(byte reg, byte value);
			void Out(byte value);

		protected:
			byte pinA1 = PIN_A1;
			byte pinA0 = PIN_A0;
			byte pinWR   = PIN_WR;
			byte pinReset = PIN_RESET;
			byte dataPins[8] = {PIN_D0,PIN_D1,PIN_D2,PIN_D3,PIN_D4,PIN_D5,PIN_D6,PIN_D7};
	};
#endif
