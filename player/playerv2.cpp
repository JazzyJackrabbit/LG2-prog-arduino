#include "playerv2.h"
#include "TimedAction.h"
#include <SoftwareSerial.h>
#include "Adafruit_NeoPixel.h"
#include "Color.h"
#include "pitches.h"
#include "musics.h"
#include "list.h"

int isInitialised_Internal = false;

bool playerIsLaserReady = false;
bool playerIsLaserSensorReady = false;
int idPlayer = 0;

Color colorOfPlayer = *new Color(0, 0, 0);

bool laserOn = false;
int laserIdCounter;

int laserSensorIdCounter = 0;
int last_laserSensorIdCounter = 0;
int n_laserSensorIdCounter = 0;
int time_playerCurrentlyDeadCounter = time_playerDead_default;

bool isPlayerKilled = false;

bool commandWaitForID1dgt = false;
bool commandWaitForID2dgt = false;
bool commandWaitForTMDTH1dgt = false;
bool commandWaitForTMDTH2dgt = false;

bool commandWaitForCOLOR = false;
int commandWaitForCOLORLEVEL = 0;

bool isSendingMessage = false;

int temp_timeDeath;
int temp_idPlayer;

int time_playerDead;

String fireButtonEvent = "IS_NOT_INIT";

int firebtnReadCalcMin = 2400;
int firebtnReadCalcMax = -400;

TimedAction threadPlayer = TimedAction(delay_playerAction, blinkPLAYER);
TimedAction threadLaser = TimedAction(delay_laserAction, blinkLASER);
TimedAction threadLaserSensor = TimedAction(delay_laserAction / delay_laserAction__laserSensorActionDivisionTime, blinkLASERSENSOR);
TimedAction threadESPCOM = TimedAction(delay_ESPCOMAction, blinkESPCOM);
TimedAction threadTimerDead = TimedAction(delay_playerAction, blinkPLAYERISDEAD);
// audio
TimedAction threadAudioPlayer = TimedAction(delay_audioFrameAction, blinkAUDIOFRAME);

bool setPlayFrame = false; // playing for 1 frame then next position
List current_melody { .length = 0, .data = { } }; // set the partition
int positionMelody = 0; // position on the array (must be pair)


// esp
SoftwareSerial serialESP(pin_RXESP, pin_TXESP); // RX, TX

// leds
Adafruit_NeoPixel pixels(maxLedNeoPixel, pin_ledsPlayerTeamColor, NEO_RGB + NEO_KHZ800);

void framePlayer() {
  if (!isInitialised_Internal) return;

  threadPlayer.check();
  threadLaser.check();
  threadLaserSensor.check();
  threadESPCOM.check();
  threadTimerDead.check();
  threadAudioPlayer.check();
}

void initPlayer()
{
  pinMode(pin_fireButton, INPUT);
  pinMode(pin_laserSensor, INPUT);
  pinMode(pin_ledsPlayerTeamColor, OUTPUT);
  pinMode(pin_laser, OUTPUT);
  pinMode(pin_audioOut, OUTPUT);

  pinMode(pin_IDOx1, INPUT);
  pinMode(pin_IDOx2, INPUT);
  pinMode(pin_IDOx3, INPUT);
  pinMode(pin_IDOx4, INPUT);
  pinMode(pin_IDOx5, INPUT);
  //pinMode(pin_IDOx6, INPUT);

  digitalWrite(pin_ledsPlayerTeamColor, LOW);
  digitalWrite(pin_laser, LOW);
  serialESP.begin(serial_baudrate_ESP);

  // time dead player
  time_playerDead = time_playerDead_default;

  // calcul ID
  setIdByPins();

  // code player here
  isNotReady();
  colorPlayerNone();
  setPlayerId(idPlayer);


  
  // ####

  isInitialised_Internal = true;
}

void setPlayerId(int _idPlayer) {
  idPlayer = _idPlayer;
  laserIdCounter = 0;
}
void isReady() {
  playerIsLaserReady = true;
  playerIsLaserSensorReady = true;
  laserIdCounter = 0;
}
void isNotReady() {
  playerIsLaserReady = false;
  playerIsLaserSensorReady = false;
}

void colorPlayer(Color clr) {

  pixels.begin();

  for (int i = maxLedNeoPixel; i > 0; i--)
    pixels.setPixelColor(i, clr.g * lightnessNeoPixel, clr.r * lightnessNeoPixel, clr.b * lightnessNeoPixel, 255);

  pixels.show();

  

}

void colorPlayerNone() {
  Color black = *new Color(0, 0, 0);
  colorPlayer(black);
}


void blinkPLAYER()
{
  int fireButton = analogRead(pin_fireButton);
  fireButtonEvents(fireButton);
}

void fireButtonEvents(int _fireButton) {

  // delta seuil
  if(enableFireButtonDelta){
    /*if(firebtnReadCalcMax - firebtnReadCalcMin < fireButtonSensitiveDelta){
      int old_firebtnReadCalcMin = firebtnReadCalcMin;
      firebtnReadCalcMin -= (firebtnReadCalcMax - firebtnReadCalcMin) /2;
      firebtnReadCalcMax += (firebtnReadCalcMax - old_firebtnReadCalcMin) /2;
    }*/
    
    if(_fireButton < firebtnReadCalcMin){
        firebtnReadCalcMax = _fireButton + fireButtonSensitiveSeuil / 2;
        firebtnReadCalcMin = _fireButton - fireButtonSensitiveSeuil / 2;
        fireButtonEvent = "GOING_OFF";
    }
    else if(_fireButton > firebtnReadCalcMax){
        firebtnReadCalcMax = _fireButton + fireButtonSensitiveSeuil / 2;
        firebtnReadCalcMin = _fireButton - fireButtonSensitiveSeuil / 2;
        fireButtonEvent = "GOING_ON";
    }

    /*Serial.print(_fireButton);
    Serial.print("   ");
    Serial.print(firebtnReadCalcMin);
    Serial.print("   ");
    Serial.print(firebtnReadCalcMax);
    Serial.print("   ");
    Serial.print(fireButtonEvent);
    Serial.println("   ");*/
   
  }
  
  // seuil
  if(enableFireButtonSeuil){
    if(_fireButton < fireButtonSensitive && fireButtonEvent == "ON"){
        fireButtonEvent = "GOING_OFF";
    }
    if(_fireButton > fireButtonSensitive && fireButtonEvent == "OFF"){
        fireButtonEvent = "GOING_ON";
    }
  }
  
  if ( fireButtonEvent == "GOING_OFF" ) {
    fireButton_GoingOFF();
    fireButtonEvent = "OFF";
  }
  if ( fireButtonEvent == "OFF" ) {
    fireButton_IsOFF();
  }
  if ( fireButtonEvent == "GOING_ON" ) {
    fireButton_GoingON();
    fireButtonEvent = "ON";
  }
  if ( fireButtonEvent == "ON" ) {
    fireButton_IsON();
  }

  
}

void fireButton_GoingON() {
  laserOn = true;
};
void fireButton_GoingOFF() {
};
void fireButton_IsON() {
};
void fireButton_IsOFF() {
  if (laserIdCounter == 0) {
    laserOn = false;
  }
};

void blinkLASER()
{
  if (playerIsLaserReady && laserOn && !isPlayerKilled ) {

    if (laserIdCounter < idPlayer) {
      digitalWrite(pin_laser, HIGH);
      laserIdCounter++;
    } else {
      digitalWrite(pin_laser, LOW);
      laserIdCounter = 0;
    }

  } else {
    digitalWrite(pin_laser, LOW);
  }



}

void blinkLASERSENSOR()
{
  if (playerIsLaserSensorReady && !isPlayerKilled ) {

    bool hasLaserOnSensor = digitalRead(pin_laserSensor);
        
    if (hasLaserOnSensor) {
      laserSensorIdCounter++;
    }
    else {
      if (laserSensorIdCounter > 0) {
        int arroundedPlayerId = arround(laserSensorIdCounter, delay_laserAction__laserSensorActionDivisionTime);
        arroundedPlayerId /= delay_laserAction__laserSensorActionDivisionTime;


        if (last_laserSensorIdCounter == arroundedPlayerId) {
          n_laserSensorIdCounter++;

          // Killed
          if (n_laserSensorIdCounter >= totalNCheck_LaserId) {

            isKilled(arroundedPlayerId);

          }
        }
        else {
          n_laserSensorIdCounter = 0;
        }

        last_laserSensorIdCounter = arroundedPlayerId;
      }

      laserSensorIdCounter = 0;
    }

  }

}

void isKilled(int _byPlayerId) {

  if (isPlayerKilled == true) return;

  // if(_byPlayerId == idPlayer) return;

  isPlayerKilled = true;

  // send killer to server


  char  msgStr[2];

  msgStr[0] = 'K';
  msgStr[3] = 'z';

  if (_byPlayerId < 10) {
    msgStr[1] = '0';
    String idStr = String(_byPlayerId);
    msgStr[2] = idStr[0];
  } else {
    char  idStr_2[1];
    itoa(_byPlayerId, idStr_2, 10);
    msgStr[1] = idStr_2[0];
    msgStr[2] = idStr_2[1];
  }

  isSendingMessage = true;

  delay(delay_ESPCOMAction);
  serialESP.write(msgStr[0]);
  delay(delay_ESPCOMAction);
  serialESP.write(msgStr[1]);
  delay(delay_ESPCOMAction);
  serialESP.write(msgStr[2]);
  delay(delay_ESPCOMAction);
  serialESP.write(msgStr[3]);

  isSendingMessage = false;

  // ####

  colorPlayerNone();
  delay(delay_flashKill);
  colorPlayer(colorOfPlayer);
  delay(delay_flashKill);

  colorPlayerNone();
  delay(delay_flashKill);
  colorPlayer(colorOfPlayer);
  delay(delay_flashKill);

  colorPlayerNone();
  delay(delay_flashKill);
  colorPlayer(colorOfPlayer);
  delay(delay_flashKill);

  colorPlayerNone();

}

int arround(int _value, int _offset) {
  int offsetD2 = _offset / 2;
  double test = ((double)_value / (double)_offset);
  test += ((double)offsetD2 / (double)_offset);
  return (int)test * _offset;
}

void blinkESPCOM()
{



  if (serialESP.available() > 0) {
    if (isSendingMessage == false) {
      char resp = serialESP.read();
      commandESP(resp);
    }
  }

}

Color bufferworkcolor = *new Color(0, 0, 0);

void commandESP(char _cmd) {
  if (_cmd == 'I') { // init
    time_playerCurrentlyDeadCounter = time_playerDead;
    isNotReady();
    setPlayerId(-1);
    colorPlayerNone();
    colorOfPlayer = *new Color(0, 0, 0);
    idPlayer = -1;
    Serial.println("Debug: INIT OK");
  }
  else if (_cmd == 'E') { // end
    time_playerCurrentlyDeadCounter = time_playerDead;
    isNotReady();
  }
  else if (_cmd == 'S') { // start
    time_playerCurrentlyDeadCounter = time_playerDead;
    colorPlayer(colorOfPlayer);
    setPlayerId(idPlayer);
    isReady();
  }
  else if (_cmd == 'A') {
    commandWaitForCOLORLEVEL = 0;
    commandWaitForCOLOR = true;
    bufferworkcolor = *new Color(0, 0, 0);
  }
  else if (_cmd == 'R') {
    colorOfPlayer = *new Color(0, 0, 0);
    colorOfPlayer.r = 200;
    colorPlayer(colorOfPlayer);
  }
  else if (_cmd == 'G') {
    colorOfPlayer = *new Color(0, 0, 0);
    colorOfPlayer.g = 200;
    colorPlayer(colorOfPlayer);
  }
  else if (_cmd == 'B') {
    colorOfPlayer = *new Color(0, 0, 0);
    colorOfPlayer.b = 200;
    colorPlayer(colorOfPlayer);
  }

  else if (_cmd == 'N') {
    colorOfPlayer = *new Color(0, 0, 0);
    colorPlayer(colorOfPlayer);
  }
  else if (_cmd == 'P') {
    isSendingMessage = true;
    setIdByPins();
    isSendingMessage = false;
  }
  else if (_cmd == 'C') { // get id

    char  msgStr[2];

    msgStr[0] = 'D';
    msgStr[3] = 'z';

    int _playerId = idPlayer;

    if (_playerId < 10) {
      msgStr[1] = '0';
      String idStr = String(_playerId);
      msgStr[2] = idStr[0];
    } else {
      char  idStr_2[1];
      itoa(_playerId, idStr_2, 10);
      msgStr[1] = idStr_2[0];
      msgStr[2] = idStr_2[1];
    }

    isSendingMessage = true;

    delay(delay_ESPCOMAction);
    serialESP.write(msgStr[0]); // send
    delay(delay_ESPCOMAction);
    serialESP.write(msgStr[1]);
    delay(delay_ESPCOMAction);
    serialESP.write(msgStr[2]);
    delay(delay_ESPCOMAction);
    serialESP.write(msgStr[3]);

    isSendingMessage = false;

  }
  else if (_cmd == 'D') {
    commandWaitForID1dgt = true;
    commandWaitForID2dgt = false;
    idPlayer = -1;
  }
  else if (_cmd == 'T') {
    commandWaitForTMDTH1dgt = true;
    commandWaitForTMDTH2dgt = false;
    time_playerDead = time_playerDead_default;
  }

  else if (isDigit(_cmd)) {
    if (commandWaitForID1dgt == true) { // affect ID to player
      int vInt = _cmd - '0';
      if (commandWaitForID2dgt == true) { // second Ox coming
        idPlayer += vInt;
        _cmd = 'z';
      }
      if (commandWaitForID2dgt == false) { // first Ox coming
        commandWaitForID2dgt = true;
        idPlayer = (vInt * 10);
      }
    }
    else if (commandWaitForTMDTH1dgt == true) {
      int vInt = _cmd - '0';
      if (commandWaitForTMDTH2dgt == true) { // second Ox coming
        time_playerDead += vInt;
        time_playerDead *= offset_playerDeadCmd;
        _cmd = 'z';
      }
      if (commandWaitForTMDTH2dgt == false) { // first Ox coming
        commandWaitForTMDTH2dgt = true;
        time_playerDead = (vInt * 10);
      }
    }
    else if (commandWaitForCOLOR == true) {
      int vInt = _cmd - '0';
      if (commandWaitForCOLORLEVEL <= 8) {

  
        if (commandWaitForCOLORLEVEL == 0) bufferworkcolor = *new Color(0, 0, 0);
        if (commandWaitForCOLORLEVEL == 0) bufferworkcolor.r += 1 * vInt * 100;
        if (commandWaitForCOLORLEVEL == 1) bufferworkcolor.r += 1 * vInt * 10;
        if (commandWaitForCOLORLEVEL == 2) bufferworkcolor.r += 1 * vInt * 1;
        if (commandWaitForCOLORLEVEL == 3) bufferworkcolor.g += 1 * vInt * 100;
        if (commandWaitForCOLORLEVEL == 4) bufferworkcolor.g += 1 * vInt * 10;
        if (commandWaitForCOLORLEVEL == 5) bufferworkcolor.g += 1 * vInt * 1;
        if (commandWaitForCOLORLEVEL == 6) bufferworkcolor.b += 1 * vInt * 100;
        if (commandWaitForCOLORLEVEL == 7) bufferworkcolor.b += 1 * vInt * 10;
        if (commandWaitForCOLORLEVEL == 8) bufferworkcolor.b += 1 * vInt * 1;

        commandWaitForCOLORLEVEL += 1;


      }

      if (commandWaitForCOLORLEVEL > 8) {
        
        colorOfPlayer = *new Color(0, 0, 0);
        colorOfPlayer.r = bufferworkcolor.r;
        colorOfPlayer.g = bufferworkcolor.g;
        colorOfPlayer.b = bufferworkcolor.b;

        colorPlayer(colorOfPlayer);
        commandWaitForCOLOR == false;
        commandWaitForCOLORLEVEL == 0;
        bufferworkcolor = *new Color(0, 0, 0);
        _cmd = 'z';
      }
    };
  }
  else {
    _cmd = 'z';
  }
  if (_cmd == 'z') {

    commandWaitForID1dgt = false;
    commandWaitForID2dgt = false;
    commandWaitForTMDTH1dgt = false;
    commandWaitForTMDTH2dgt = false;
    commandWaitForCOLORLEVEL = 0;
    commandWaitForCOLOR = false;
  }


}

void blinkPLAYERISDEAD() {
  if (isPlayerKilled) {
    if (time_playerCurrentlyDeadCounter > 0) {
      time_playerCurrentlyDeadCounter -= 1;
    }
    else {

      colorPlayer(colorOfPlayer);

      isPlayerKilled = false;
      time_playerCurrentlyDeadCounter = time_playerDead;

    }
  }
}

void setIdByPins() {
  int resultId = 0;

  int int1 = digitalRead(pin_IDOx1);
  int int2 = digitalRead(pin_IDOx2);
  int int4 = digitalRead(pin_IDOx3);
  int int8 = digitalRead(pin_IDOx4);
  int int16 = digitalRead(pin_IDOx5);
  //int int32 = digitalRead(pin_IDOx6);

  if (int1 > 0) resultId += 1;
  if (int2 > 0) resultId += 2;
  if (int4 > 0) resultId += 4;
  if (int8 > 0) resultId += 8;
  if (int16 > 0) resultId += 16;
  //if (int32 > 0) resultId += 32;

  resultId += startIdPlayerOffset;
  idPlayer = 0;
  idPlayer = resultId;

}

// AUDIO:

void blinkAUDIOFRAME() {
   if (!playerIsLaserReady || !laserOn || isPlayerKilled ) return;
   
  if(setPlayFrame){
    //play();
    int noteDuration = delay_audioFrameAction;
    int note = current_melody.data[positionMelody+0];
    tone(pin_audioOut, note, noteDuration);
    if(note == 0)
       noTone(pin_audioOut);
    
    positionMelody+=1;
    if(positionMelody >= current_melody.length){
      positionMelody = 0;
      setPlayFrame = false;
      noTone(pin_audioOut);
    }
  }

  threadAudioPlayer = TimedAction(delay_audioFrameAction, blinkAUDIOFRAME);
}
