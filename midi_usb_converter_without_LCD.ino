
/*
* ********************************************************
* Converter that sends midi messages from a 
* conventional 5pin din connection (midi-in)
* to the USB shield output.
* ********************************************************
* 
*/

// include the library code:
#include <MIDI.h>
#include <usbh_midi.h>

/*   USB ERROR CODES
 hrSUCCESS   0x00
 hrBUSY      0x01
 hrBADREQ    0x02
 hrUNDEF     0x03
 hrNAK       0x04
 hrSTALL     0x05
 hrTOGERR    0x06
 hrWRONGPID  0x07
 hrBADBC     0x08
 hrPIDERR    0x09
 hrPKTERR    0x0A
 hrCRCERR    0x0B
 hrKERR      0x0C
 hrJERR      0x0D
 hrTIMEOUT   0x0E
 hrBABBLE    0x0F
 */

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

//Arduino MIDI library v4.2 compatibility
#ifdef MIDI_CREATE_DEFAULT_INSTANCE
MIDI_CREATE_DEFAULT_INSTANCE();
#endif
#ifdef USBCON
#define _MIDI_SERIAL_PORT Serial1
#else
#define _MIDI_SERIAL_PORT Serial
#endif

//include USB library code
USB  Usb;
USBH_MIDI  Midi(&Usb);

//void MIDI_poll();
void doDelay(uint32_t t1, uint32_t t2, uint32_t delayTime);

byte NoteOn = 0x90;
byte NoteOff = 0x80;
byte ControlChange = 0xB0;
byte ProgramChange = 0xC0;
uint16_t pid, vid;
byte lastState = 0;
bool ready = false;
int finished = 1;
int delayvalue = 50;
int receivedelay = delayvalue;

// sends CC value 
void SendCC(byte CHAN, byte NR, byte VAL) {
//  MIDI.sendControlChange(NR, VAL, CHAN);
   uint8_t msg[4];
   msg[0] = ControlChange + CHAN;
   msg[1] = NR;
   msg[2] = VAL;
if (isReady()) {
   Midi.SendData(msg, 0);
   }
}

// Sends progchange command
void SendPP(byte CHAN, byte PRG) {
   uint8_t msg[2];
   msg[0] = ProgramChange + CHAN;
   msg[1] = PRG;
if (isReady()) {
   Midi.SendData(msg, 0);
   }
}

// Sends NoteOn command 
void SendNoteOn(uint8_t CHAN, uint8_t NOTE, uint8_t VEL) {

uint8_t msg[3];
msg[0] = NoteOn + CHAN;
msg[1] = NOTE;
msg[2] = VEL;
if (isReady()) {
   Midi.SendData(msg, 0);
   finished = 0;
   receivedelay = delayvalue;
   }
}

// Sends NoteOff command
void SendNoteOff(byte CHAN, byte NOTE, byte VEL) {
 
 uint8_t msg[3];
 msg[0] = NoteOff + CHAN;
 msg[1] = NOTE;
 msg[2] = VEL;
 if (isReady()) {
   Midi.SendData(msg, 0);
   finished = 0;
   receivedelay = delayvalue;
   }
}  

// Sends Sysex message 
void SendSysEx(byte ARRAY, byte SIZE) {
if (isReady()) {
  MIDI.sendSysEx(SIZE, ARRAY, true);
  }
}

void SendAllOff () {
      uint8_t msg[3];
      for (int nte=1; nte < 127; nte++){
         for (int cha=1; cha < 16; cha++){        
           msg[0] = NoteOff + cha;
           msg[1] = nte;
           msg[2] = 0;
           if (isReady()) {
              Midi.SendData(msg, 0);
           }
         }
        }
    finished = 1;
}

void doDelay(uint32_t t1, uint32_t t2, uint32_t delayTime)
{
  uint32_t t3;

  if ( t1 > t2 ) {
    t3 = (0xFFFFFFFF - t1 + t2);
  } else {
    t3 = t2 - t1;
  }

  if ( t3 < delayTime ) {
    delayMicroseconds(delayTime - t3);
  }
}

bool isReady() {
   Usb.Task();
   if (Usb.getUsbTaskState() == USB_STATE_RUNNING) {
          return true;
      } else if (lastState != Usb.getUsbTaskState()) {
          return false;
      }
   return false;            
 }

void SignalHigh() {
          digitalWrite(LED_BUILTIN, HIGH);
}

void SignalLow() {
          digitalWrite(LED_BUILTIN, LOW);
}

void setup() {
  vid = pid = 0;
  int ledPin = 13;
  pinMode(LED_BUILTIN, OUTPUT);

// check if usb init 
 if (Usb.Init() == -1) {
      for (int i=0; i <= 10; i++){
          digitalWrite(LED_BUILTIN, HIGH);
          delay(500);
          digitalWrite(LED_BUILTIN, LOW);
          delay(1000);
      }
      while(1);
    
  }//if (Usb.Init() == -1...

  delay( 100 );

// Handles for incoming midi messages 
 MIDI.setHandleControlChange(SendCC);
 MIDI.setHandleProgramChange(SendPP);
 MIDI.setHandleNoteOn(SendNoteOn);
 MIDI.setHandleNoteOff(SendNoteOff);
 MIDI.setHandleSystemExclusive(SendSysEx);  
 MIDI.begin();
}

void loop() {
  uint32_t t1 = (uint32_t)micros();
  MIDI.read(MIDI_CHANNEL_OMNI); 
 /* 
  if (finished == 0) {
    --receivedelay;
    if (receivedelay == 0) {
      SendAllOff();
    }
  }
// doDelay(t1, (uint32_t)micros(), 1000);
*/ 
}
