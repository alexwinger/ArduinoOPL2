#include <SPI.h>
#include <OPL2.h>
#include <midi_instruments.h>

#define CONTROL_VOLUME          7
#define CONTROL_ALL_SOUND_OFF 120
#define CONTROL_RESET_ALL     121
#define CONTROL_ALL_NOTES_OFF 123

#define MIDI_CHANNELS 16
#define OPL2_CHANNELS 9

const unsigned char *instruments[] = {
  PIANO1, PIANO2, PIANO3, HONKTONK, EP1, EP2, HARPSIC, CLAVIC, CELESTA, GLOCK, MUSICBOX, VIBES, MARIMBA, XYLO, TUBEBELL, SANTUR,
  ORGAN1, ORGAN2, ORGAN3, PIPEORG, REEDORG, ACORDIAN, HARMONIC, BANDNEON, NYLONGT, STEELGT, JAZZGT, CLEANGT, MUTEGT, OVERDGT, DISTGT, GTHARMS,
  ACOUBASS, FINGBASS, PICKBASS, FRETLESS, SLAPBAS1, SLAPBAS2, SYNBASS1, SYNBASS2, VIOLIN, VIOLA, CELLO, CONTRAB, TREMSTR, PIZZ, HARP, TIMPANI,
  STRINGS, SLOWSTR, SYNSTR1, SYNSTR2, CHOIR, OOHS, SYNVOX, ORCHIT, TRUMPET, TROMBONE, TUBA, MUTETRP, FRHORN, BRASS1, SYNBRAS1, SYNBRAS2,
  SOPSAX, ALTOSAX, TENSAX, BARISAX, OBOE, ENGLHORN, BASSOON, CLARINET, PICCOLO, FLUTE1, RECORDER, PANFLUTE, BOTTLEB, SHAKU, WHISTLE, OCARINA,
  SQUARWAV, SAWWAV, SYNCALLI, CHIFLEAD, CHARANG, SOLOVOX, FIFTHSAW, BASSLEAD, FANTASIA, WARMPAD, POLYSYN, SPACEVOX, BOWEDGLS, METALPAD, HALOPAD, SWEEPPAD,
  ICERAIN, SOUNDTRK, CRYSTAL, ATMOSPH, BRIGHT, GOBLIN, ECHODROP, STARTHEM, SITAR, BANJO, SHAMISEN, KOTO, KALIMBA, BAGPIPE, FIDDLE, SHANNAI,
  TINKLBEL, AGOGO, STEELDRM, WOODBLOK, TAIKO, MELOTOM, SYNDRUM, REVRSCYM, FRETNOIS, BRTHNOIS, SEASHORE, BIRDS, TELEPHON, HELICOPT, APPLAUSE
};

struct ChannelMapping {
  byte midiChannel;
  byte midiNote;
  float midiVelocity;
  float op1Level;
  float op2Level;
};


OPL2 opl2;
ChannelMapping channelMap[OPL2_CHANNELS];
byte lastUsed[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
byte programMap[MIDI_CHANNELS];
float channelVolumes[MIDI_CHANNELS];


void setup() {
  // Serial.begin(115200);

  usbMIDI.setHandleNoteOn(onNoteOn);
  usbMIDI.setHandleNoteOff(onNoteOff);
  usbMIDI.setHandleProgramChange(onProgramChange);
  usbMIDI.setHandlePitchChange(onPitchChange);
  usbMIDI.setHandleControlChange(onControlChange);
  usbMIDI.setHandleSystemReset(onSystemReset);
  onSystemReset();
}


void loop() {
  usbMIDI.read();
}


byte getFreeChannel(byte midiChannel) {  
  byte opl2Channel = 255;

  for (byte i = 0; i < OPL2_CHANNELS; i ++) {
    if (!opl2.getKeyOn(i)) {
      opl2Channel = i;
      break;
    }
  }

  if (opl2Channel == 255) {
    opl2Channel = lastUsed[0];
  }

  /**
   * Update the list of last used channels by moving the current channel to the
   * bottom so the last updated channel will move to the front of the list. If no
   * more OPL2 channels are free then the last updated one will be recycled.
   */
  byte i;
  for (i = 0; i < OPL2_CHANNELS && lastUsed[i] != opl2Channel; i ++) {}

  while (i < OPL2_CHANNELS - 1) {
    byte temp = lastUsed[i + 1];
    lastUsed[i + 1] = lastUsed[i];
    lastUsed[i] = temp;
    i ++;
  }

  return opl2Channel;
}


void setOpl2ChannelVolume(byte opl2Channel, byte midiChannel) {
  float volume = channelMap[opl2Channel].midiVelocity * channelVolumes[midiChannel];
  byte volumeOp1 = round(channelMap[opl2Channel].op1Level * volume * 63.0);
  byte volumeOp2 = round(channelMap[opl2Channel].op2Level * volume * 63.0);
  opl2.setVolume(opl2Channel, OPERATOR1, 63 - volumeOp1);
  opl2.setVolume(opl2Channel, OPERATOR2, 63 - volumeOp2);
}


void onNoteOn(byte channel, byte note, byte velocity) {
  // Treat notes with a velocity of 0 as note off.
  if (velocity == 0) {
    onNoteOff(channel, note, velocity);
    return;
  }
  
  if (channel == 10) return;

  // Get an available OPL2 channel and setup instrument parameters.
  byte i = getFreeChannel(channel);
  opl2.setInstrument(i, instruments[programMap[channel]]);
  channelMap[i].midiChannel  = channel;
  channelMap[i].midiNote     = note;
  channelMap[i].midiVelocity = round(velocity / 127.0);
  channelMap[i].op1Level     = round((63 - opl2.getVolume(i, OPERATOR1)) / 63.0);
  channelMap[i].op2Level     = round((63 - opl2.getVolume(i, OPERATOR2)) / 63.0);

  // Set operator output levels based on note velocity.
  setOpl2ChannelVolume(i, channel);

  // Calculate octave and note number and play note!
  note = max(24, min(note, 119));
  byte opl2Octave = 1 + (note - 24) / 12;
  byte opl2Note   = note % 12;
  opl2.playNote(i, opl2Octave, opl2Note);
}


void onNoteOff(byte channel, byte note, byte velocity) {
  for (byte i = 0; i < OPL2_CHANNELS; i ++) {
    if (channelMap[i].midiChannel == channel && channelMap[i].midiNote == note) {
      opl2.setKeyOn(i, false);
      break;
    }
  }
}


void onProgramChange(byte channel, byte program) {
  programMap[channel] = program;
}


void onPitchChange(byte channel, int pitch) {
  // Serial.print("Pitch change on channel ");
  // Serial.print(channel);
  // Serial.print(" to ");
  // Serial.println(pitch);
}


void onControlChange(byte channel, byte control, byte value) {
  switch (control) {

    // Change volume of a MIDI channel.
    case CONTROL_VOLUME: {
      channelVolumes[channel] = round(min(value, 63) / 63.0);

      for (byte i = 0; i < OPL2_CHANNELS; i ++) {
        if (channelMap[i].midiChannel == channel && opl2.getKeyOn(i)) {
          setOpl2ChannelVolume(i, channel);
        }
      }
      break;
    }

    // Reset all controller values.
    case CONTROL_RESET_ALL:
      for (byte i = 0; i < MIDI_CHANNELS; i ++) {
        channelVolumes[channel] = 0.8;
      }
    break;

    // Silence all MIDI channels.
    case CONTROL_ALL_SOUND_OFF:
    case CONTROL_ALL_NOTES_OFF: {
      for (byte i = 0; i < OPL2_CHANNELS; i ++) {
        if (channelMap[i].midiChannel == channel) {
          onNoteOff(channelMap[i].midiChannel, channelMap[i].midiNote, 0);
        }
      }
      break;
    }

    default:
      break;
  }
}


void onSystemReset() {
  opl2.init();
  for (byte i = 0; i < OPL2_CHANNELS; i ++) {
    opl2.setKeyOn(i, false);
    opl2.setInstrument(i, PIANO1);
  }

  for (byte i = 0; i < MIDI_CHANNELS; i ++) {
    programMap[i] = 0;
    channelVolumes[i] = 0.8;
  }
}
