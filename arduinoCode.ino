#include <LedControl.h>

//Pin Mappings (final)
// 0 A9 (9) - Note1 (C)
// 1 A8 (8) - Note2 (Cs)
// 2 A7 (7) - Note3 (D)
// 3 A6 (6) - Note4 (Ds)
// 4 A5 (5) - Note5 (E)
// 5 A4 (4) - Note6 (F)
// 6 A3 (3) - Note7 (Fs)
// 7 A2 (2) - Note8 (G)
// 8 A1 (1) - Note9 (Gs)
// 9 A0 (0) - Note10 (A)
// 10 A22 (67) - Note11 (As)
// 11 A21 (66) - Note12 (B)
// 12 A20 (39) - Note13 (C)
// 13 A19 (38) - Octave Down
// 14 A18 (37) - Middle C
// 15 A17 (36) - Octave Up
// 16 A13 (32) - Toggle Vibrato
// 17 A12 (31) - Toggle Octave

// A10 (64)
// A11 (65)
// A12 (31)
// A13 (32)
// A14 (33)
// A15 (34)
// A16 (35)
// A17 (36)
// A18 (37)
// A19 (38)

int thresh = 100;
byte flatName = B11111;
byte sharpName = B1011011;

int pinNum[18] = {9,8,7,6,5,4,3,2,1,0,67,66,39,38,37,36,32,31};
//int pinNum[6] = {9,8,7,6,5,31};
//int pinNum[17] = {9,8,7,6,5,4,3,2,1,0,67,66,39,38,37,36,31};
unsigned char CKeyNote[13] = {60,61,62,63,64,65,66,67,68,69,70,71,72};
//unsigned char CKeyNote[13] = {60,61,62,63,64,65,66,67,68,69,70,71,72};
byte CKeyNoteName[16] = {0x4E,0,0x7E,0,0x4F,0,0x47,0,0x5F,0,0x77,0,0x7F,0,0x4E,0};
byte CKeyFNoteName[16] = {0x4E,flatName,0x7E,flatName,0x4F,flatName,0x47,flatName,0x5F,flatName,0x77,flatName,0x7F,flatName,0x4E,flatName};
byte CKeySNoteName[16] = {0x4E,sharpName,0x7E,sharpName,0x4F,sharpName,0x47,sharpName,0x5F,sharpName,0x77,sharpName,0x7F,sharpName,0x4E,sharpName};

unsigned char KeyNote[8];
bool NoteOn[86];
//bool NoteOn[13] = {false, false, false, false, false, false, false, false, false, false, false, false, false};
bool NoteVar[3][3] = 
{
  {13,12,11},
  {1,0,-1},
  {-11,-12,-13}
};

//     | shar | natu | flat
// up  |  13  |  12  |  11 
// 0   |   1  |   0  |  -1
// dow | -11  | -12  | -13
int octArr = 0;
int accArr = 0;

bool volControl = true;
bool cKey = true;
bool fKey = false;
bool bfKey = false;
const int sharpThresh = 700;
const int flatThresh = 350;
bool flat = false;
bool sharp = false;
bool octDown = false;
bool octUp = false;
bool octDowned = false;
bool octUped = false;
int octaveShift = 0;
int togOctaveShift = 0;
unsigned long lastOctaveDown;
unsigned long lastOctaveUp;

LedControl left = LedControl(0, 2, 1, 1);
LedControl right = LedControl(3, 5, 4, 1);

void setup() {
  // put your setup code her=e, to run once:
  Serial.begin(9600);
  left.shutdown(0, false);  // turns on display
  left.setIntensity(0, 15); // 15 = brightest
  left.clearDisplay(0);
  right.shutdown(0, false);  // turns on display
  right.setIntensity(0, 15); // 15 = brightest
  right.clearDisplay(0);
}

void loop() {
  int volt;
  int note = 0;

  double vol = 0;

  for (int i = 0; i < 18; i++) {
    volt = analogRead(pinNum[i]);
    if (i == 16) { //toggle vibrato
        if (volt <= flatThresh) {
          Serial.println("flat");
          sharp = false;
          flat = true;
          accArr = 2;
        }
        else if (volt >= sharpThresh) {
          Serial.println("sharp");
          sharp = true;
          flat = false;
          accArr = 0;
        }
        else {
          sharp = false;
          flat = false;
          accArr = 1;
        }
    }
    else if (i == 17) { //toggle octave
        if (volt <= flatThresh) {
          octUp = false;
          octDown = true;
          octArr = 2;
        }
        else if (volt >= sharpThresh) {
          octUp = true;
          octDown = false;
          octArr = 0;
        }
        else {
          octUp = false;
          octDown = false;
          octArr = 1;
        }
    }
    else if (i == 13) {
      if (octaveShift > -3 && volt >= thresh) {
        Serial.println("octave down button");
        if (lastOctaveDown == 0 || ((millis() - lastOctaveDown) > 200)) {
          octaveShift -= 1;
        }
       lastOctaveDown = millis();
      }
    }
    else if (i == 14) {
      if (volt >= thresh) {
//          Serial.println("middle C button");
        octaveShift = 0;
      }
    }
    else if (i == 15) {
      if (octaveShift <= 3 && volt >= thresh) {
        Serial.println("octave up button");
        if (lastOctaveUp == 0 || ((millis() - lastOctaveUp) > 200)) {
          octaveShift += 1;
        }
       lastOctaveUp = millis();
      }
    }
    else {
      if (sharp) {
          showNotes(CKeySNoteName);
      }
      else if (flat) {
          showNotes(CKeyFNoteName);
      }
      else {
          showNotes(CKeyNoteName); 
      }
//      if (octDown) {
//        if (!octDowned) {
//          togOctaveShift = -1;
//          octDowned = true;
//          Serial.println("octave down");
//        }
//      }
//      else if (octUp) {
//        if (!octUped) {
//          togOctaveShift = 1;
//          octUped = true;
//          Serial.println("octave up");
//        }
//      }
//      else {
//        octUped = false;
//        octDowned = false;
//        togOctaveShift = 0;
//      }
      vol = (volt+100.00)/(700.00) * 100.00;
//      note = CKeyNote[i] + 12*(octaveShift+togOctaveShift);
      note = CKeyNote[i] + NoteVar[octArr][accArr];
      if (volt >= thresh) {
//        Serial.println(i);
//        Serial.println(note);
        for (int o = 0; o < 3; o++) {
          for (int a = 0; a <3; a++) {
            int testNote = CKeyNote[i] + NoteVar[o][a];
            if (o == octArr && a == accArr) {
              if (NoteOn[testNote-21]) { //already playing
                usbMIDI.sendControlChange(7,vol,i);
                Serial.println("hold note");
                Serial.println(note);
              }
              else {
                Serial.println("play note");
                Serial.println(note);
                usbMIDI.sendNoteOn(testNote,vol,i);
                NoteOn[testNote-21] = true;
              }
            }
            else {
              NoteOn[testNote-21] = false;
              usbMIDI.sendNoteOff(testNote,vol,i);
            }
          }
        }


        
//          if (NoteOn[note - 21]) {
            
//            int noteOffOct = 0;
//            if (octUped) {
//              noteOffOct = -12;
//            }
//            else if (octDowned) {
//              noteOffOct = 12;
//            }
//            else {
//              noteOffOct=0;
//            }
//            usbMIDI.sendControlChange(7,vol,i);
//            int newNote = note;
//            int oldNote = note;
//            if (sharp) {
//              newNote += 1;
//              oldNote -= 1;   
//            }
//            else if (flat) {
//              newNote -= 1;
//              oldNote += 1;
//            }
//            if (octUp) {
//              newNote += 12;
//              oldNote -= 12;
//            }
//            else if (octDown) {
//              newNote -= 12;
//              oldNote += 12;
//            }
//            if (sharp || flat || octUp || octDown) {
//              usbMIDI.sendNoteOn(newNote,vol,i);
//              usbMIDI.sendNoteOff(oldNote,vol,i);
//              usbMIDI.sendNoteOff(note,vol,i);
//            }
//            else {
//              usbMIDI.sendNoteOn(note,vol,i);
//              usbMIDI.sendControlChange(7,vol,i);
//              usbMIDI.
//            }
            
//            if (sharp) {
//              usbMIDI.sendNoteOff(note,vol,i); 
//              usbMIDI.sendNoteOff(note-1,vol,i); 
//              usbMIDI.sendNoteOn(note+1,vol,i);
//              usbMIDI.sendNoteOff(note+noteOffOct,vol,i); 
//              usbMIDI.sendNoteOff(note-1+noteOffOct,vol,i);
//            }
//            else if (flat) {
//              usbMIDI.sendNoteOff(note,vol,i); 
//              usbMIDI.sendNoteOff(note+1,vol,i); 
//              usbMIDI.sendNoteOn(note-1,vol,i);
//              usbMIDI.sendNoteOff(note+noteOffOct,vol,i); 
//              usbMIDI.sendNoteOff(note+1+noteOffOct,vol,i);
//            }
//            else {
//              usbMIDI.sendNoteOn(note,vol,i);
//              usbMIDI.sendControlChange(7,vol,i);
//              usbMIDI.sendNoteOff(note+1,vol,i); 
//              usbMIDI.sendNoteOff(note-1,vol,i); 
//              usbMIDI.sendNoteOff(note+1+noteOffOct,vol,i); 
//              usbMIDI.sendNoteOff(note-1+noteOffOct,vol,i); 
//            }
//          }
//          else {
//            NoteOn[i] = true;
//            usbMIDI.sendNoteOn(note,vol,i);
//          }
      }
      else {
        if (NoteOn[note-21]) {          
          usbMIDI.sendNoteOff(note,vol,i);
          NoteOn[note-21] = false;
        }
      }
    }
  }
  delay(50);
}

void showNotes(byte notes[]){
  for (int i = 0; i < 8; i++) {
    left.setRow(0, 7-i, notes[i]);
  }
  for (int i = 0; i < 8; i++) {
    right.setRow(0, 7-i, notes[i+8]);
  }
}

