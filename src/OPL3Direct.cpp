/**
 * ________ __________.____    ________      _____            .___.__         .____    ._____.
 * \_____  \\______   \    |   \_____  \    /  _  \  __ __  __| _/|__| ____   |    |   |__\_ |__
 *  /   |   \|     ___/    |    /  ____/   /  /_\  \|  |  \/ __ | |  |/  _ \  |    |   |  || __ \
 * /    |    \    |   |    |___/       \  /    |    \  |  / /_/ | |  (  <_> ) |    |___|  || \_\ \
 * \_______  /____|   |_______ \_______ \ \____|__  /____/\____ | |__|\____/  |_______ \__||___  /
 *         \/                 \/       \/         \/           \/                     \/       \/
 *   ___________         __                       .__                         _____
 *   \_   _____/__  ____/  |_  ____   ____   _____|__| ____   ____   ______ _/ ____\___________
 *    |    __)_\  \/  /\   __\/ __ \ /    \ /  ___/  |/  _ \ /    \ /  ___/ \   __\/  _ \_  __ \
 *    |        \>    <  |  | \  ___/|   |  \\___ \|  (  <_> )   |  \\___ \   |  | (  <_> )  | \/
 *   /_______  /__/\_ \ |__|  \___  >___|  /____  >__|\____/|___|  /____  >  |__|  \____/|__|
 *           \/      \/           \/     \/     \/               \/     \/
 *                ________ __________.____     ________   ________              ._.
 *                \_____  \\______   \    |    \_____  \  \______ \  __ __  ____| |
 *                 /   |   \|     ___/    |      _(__  <   |    |  \|  |  \/  _ \ |
 *                /    |    \    |   |    |___  /       \  |    `   \  |  (  <_> )|
 *                \_______  /____|   |_______ \/______  / /_______  /____/ \____/__
 *                        \/                 \/       \/          \/             \/
 *
 * Extensions to the OPL2 Audio Library for Arduino and compatibles to support the OPL3 Duo! board.
 * This file contains the extensions required for dual YMF262.
 * Code by Maarten janssen (maarten@cheerful.nl) 2020-10-12
 * WWW.CHEERFUL.NL
 */


#include "OPL3Direct.h"
/**
 * Create a new OPL3Direct instance with default pins.
 *
*/
OPL3Direct::OPL3Direct() : OPL3() {
}

/**
 * Create a new OPL3Direct instance with specified pins.
 *
*/
OPL3Direct::OPL3Direct(byte a1, byte a0,byte wr, byte reset, byte d0,byte d1,byte d2, byte d3, byte d4, byte d5,byte d6,byte d7){
	pinA1=a1;
	pinA0=a0;
	pinWR=wr;
	pinReset=reset;
	dataPins[0]=d0;
	dataPins[1]=d1;
	dataPins[2]=d2;
	dataPins[3]=d3;
	dataPins[4]=d4;
	dataPins[5]=d5;
	dataPins[6]=d6;
	dataPins[7]=d7;
}
/**
 * Initialize interface to OPL3Direct.
 */
void OPL3Direct::setupInterface() {
  #ifdef OPL_SERIAL_DEBUG
    Serial.begin(115200);
    while(!Serial);
    Serial.println("//OPL serial debug enabled");
  #endif
  pinMode(pinA1, OUTPUT);
  pinMode(pinA0, OUTPUT);
  pinMode(pinWR, OUTPUT);
  pinMode(pinReset, OUTPUT);
  digitalWrite(pinA1, LOW);
  digitalWrite(pinA0, LOW);
  digitalWrite(pinWR, HIGH);
  digitalWrite(pinReset, HIGH);
  
    pinMode(PIN_D0,OUTPUT);
    pinMode(PIN_D1,OUTPUT);
    pinMode(PIN_D2,OUTPUT);
    pinMode(PIN_D3,OUTPUT);
    pinMode(PIN_D4,OUTPUT);
    pinMode(PIN_D5,OUTPUT);
    pinMode(PIN_D6,OUTPUT);
    pinMode(PIN_D7,OUTPUT);
}

void OPL3Direct::hardReset(){
	// Hard reset both OPL3 chips.
	for (byte unit = 0; unit < 2; unit ++) {
		digitalWrite(pinReset, LOW);
		delay(1);
		digitalWrite(pinReset, HIGH);
	}
}

void OPL3Direct::Out(byte value){
    digitalWrite(PIN_D0,(value&  1)?HIGH:LOW);
    digitalWrite(PIN_D1,(value&  2)?HIGH:LOW);
    digitalWrite(PIN_D2,(value&  4)?HIGH:LOW);
    digitalWrite(PIN_D3,(value&  8)?HIGH:LOW);
    digitalWrite(PIN_D4,(value& 16)?HIGH:LOW);
    digitalWrite(PIN_D5,(value& 32)?HIGH:LOW);
    digitalWrite(PIN_D6,(value& 64)?HIGH:LOW);
    digitalWrite(PIN_D7,(value&128)?HIGH:LOW);  
}


/**
 * Write a given value to a register of the selected OPL3 chip.
 *
 * @param reg - The register to be changed.
 * @param value - The value to write to the register.
 */
void OPL3Direct::write(byte bank, byte reg, byte value) {
  #ifdef OPL_SERIAL_DEBUG
    Serial.print("SetReg(0x");
    Serial.print(bank);
    if(reg<16)Serial.print(0);
    Serial.print(reg, HEX);
    Serial.print(", 0x");
    Serial.print(value, HEX);
    Serial.println(");");
  #endif
	  
  digitalWrite(pinA1, (bank & 0x1) ? HIGH : LOW);
  write( reg, value);
}

/**
 * Write a given value to a register of the selected OPL3 chip.
 *
 * @param reg - The register to be changed.
 * @param value - The value to write to the register.
 */
void OPL3Direct::write(byte reg, byte value) {
  
  digitalWrite(pinA0, LOW);
  delayMicroseconds(8);
  digitalWrite(pinWR, LOW);
  Out(reg);
  delayMicroseconds(8);
  digitalWrite(pinWR, HIGH);
  delayMicroseconds(8);

  digitalWrite(pinA0, HIGH);
  delayMicroseconds(8);
  digitalWrite(pinWR, LOW);
  Out(value);
  delayMicroseconds(8);
  digitalWrite(pinWR, HIGH);
  delayMicroseconds(8);
}
