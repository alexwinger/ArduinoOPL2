/**
 * ________ __________.____    ________      _____            .___.__         .____    ._____.
 * \_____  \\______   \    |   \_____  \    /  _  \  __ __  __| _/|__| ____   |    |   |__\_ |__
 *  /   |   \|     ___/    |    /  ____/   /  /_\  \|  |  \/ __ | |  |/  _ \  |    |   |  || __ \
 * /    |    \    |   |    |___/       \  /    |    \  |  / /_/ | |  (  <_> ) |    |___|  || \_\ \
 * \_______  /____|   |_______ \_______ \ \____|__  /____/\____ | |__|\____/  |_______ \__||___  /
 *         \/                 \/       \/ _____   \/           \/                     \/       \/
 *                                      _/ ____\___________
 *                                      \   __\/  _ \_  __ \
 *                                       |  | (  <_> )  | \/
 *                                       |__|  \____/|__|
 *               _____            .___    .__                  ____    __________.__
 *              /  _  \_______  __| _/_ __|__| ____   ____    /  _ \   \______   \__|
 *             /  /_\  \_  __ \/ __ |  |  \  |/    \ /  _ \   >  _ </\  |     ___/  |
 *            /    |    \  | \/ /_/ |  |  /  |   |  (  <_> ) /  <_\ \/  |    |   |  |
 *            \____|__  /__|  \____ |____/|__|___|  /\____/  \_____\ \  |____|   |__|
 *                    \/           \/             \/                \/
 *
 * YM3812 OPL2 Audio Library for Arduino, Raspberry Pi and Orange Pi v2.1.1
 * Code by Maarten Janssen (maarten@cheerful.nl) 2016-12-18
 * WWW.CHEERFUL.NL
 *
 * Look for example code on how to use this library in the examples folder.
 *
 * Connect the OPL2 Audio Board as follows. To learn how to connect your favorite development platform visit the wiki at
 * https://github.com/DhrBaksteen/ArduinoOPL2/wiki/Connecting.
 *    OPL2 Board | Arduino | Raspberry Pi
 *   ------------+---------+--------------
 *      Reset    |    8    |      18
 *      A0       |    9    |      16
 *      Latch    |   10    |      12
 *      Data     |   11    |      19
 *      Shift    |   13    |      23
 *
 *
 * IMPORTANT: Make sure you set the correct BOARD_TYPE in OPL2.h. Default is set to Arduino.
 *
 *
 * Last updated 2021-07-11
 * Most recent version of the library can be found at my GitHub: https://github.com/DhrBaksteen/ArduinoOPL2
 * Details about the YM3812 and YMF262 chips can be found at http://www.shikadi.net/moddingwiki/OPL_chip
 *
 * This library is open source and provided as is under the MIT software license, a copy of which is provided as part of
 * the project's repository. This library makes use of Gordon Henderson's Wiring Pi.
 * WWW.CHEERFUL.NL
 */


#include <OPL2AB.h>

#if BOARD_TYPE == OPL2_BOARD_TYPE_ARDUINO
	#include <SPI.h>
	#include <Arduino.h>
#else
	#include <wiringPi.h>
	#include <wiringPiSPI.h>
#endif


/**
 * Instantiate the OPL2 library with default pin setup.
 */
OPL2AB::OPL2AB() {
	#if BOARD_TYPE == OPL2_BOARD_TYPE_RASPBERRY_PI
		wiringPiSetup();
	#endif
}


/**
 * Instantiate the OPL2 library with custom pin setup. This constructor is left for legacy support. Preferably custom
 * pins are specifies when calling begin().
 *
 * @param reset - Pin number to use for RESET.
 * @param address - Pin number to use for A0.
 * @param latch - Pin number to use for LATCH.
 */
OPL2AB::OPL2AB(byte reset, byte address, byte latch) : OPL2::OPL2() {
	pinReset   = reset;
	pinAddress = address;
	pinLatch   = latch;
}


/**
 * Initialize interface to OPL2 Audio Board.
 */
void OPL2AB::setupInterface() {
	#ifdef OPL_SERIAL_DEBUG
		Serial.begin(115200);
		while(!Serial);
		Serial.println("OPL serial debug enabled");
	#endif

	#if BOARD_TYPE == OPL2_BOARD_TYPE_ARDUINO
		SPI.begin();
		SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
	#else
		wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED);
	#endif

	pinMode(pinLatch,   OUTPUT);
	pinMode(pinAddress, OUTPUT);
	pinMode(pinReset,   OUTPUT);

	digitalWrite(pinLatch,   HIGH);
	digitalWrite(pinReset,   HIGH);
	digitalWrite(pinAddress, LOW);
}

/**
 * Hard reset the OPL2 chip
 */
void OPL2AB::hardReset(){
  // Hard reset both OPL2 chip.
    digitalWrite(pinReset, LOW);
    delay(1);
    digitalWrite(pinReset, HIGH);
}

/**
 * Write the given value to an OPL2 register. This does not update the internal shadow register!
 *
 * @param reg - The register to change.
 * @param value - The value to write to the register.
 */
void OPL2AB::write(byte reg, byte value) {
	#ifdef OPL_SERIAL_DEBUG
		Serial.print("reg: ");
		Serial.print(reg, HEX);
		Serial.print(", val: ");
		Serial.println(value, HEX);
	#endif

	// Write OPL2 address.
	digitalWrite(pinAddress, LOW);
	#if BOARD_TYPE == OPL2_BOARD_TYPE_ARDUINO
		SPI.transfer(reg);
	#else
		wiringPiSPIDataRW(SPI_CHANNEL, &reg, 1);
	#endif
	digitalWrite(pinLatch, LOW);
	delayMicroseconds(16);
	digitalWrite(pinLatch, HIGH);
	delayMicroseconds(16);

	// Write OPL2 data.
	digitalWrite(pinAddress, HIGH);
	#if BOARD_TYPE == OPL2_BOARD_TYPE_ARDUINO
		SPI.transfer(value);
	#else
		wiringPiSPIDataRW(SPI_CHANNEL, &value, 1);
	#endif
	digitalWrite(pinLatch, LOW);
	delayMicroseconds(4);
	digitalWrite(pinLatch, HIGH);
	delayMicroseconds(92);
}
