#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H
#include <Arduino.h>
#include "Color.h"

//#define pin_ledsPlayerTeamBlue  //12 //9
//#define pin_ledsPlayerTeamRed   //10
#define pin_ledsPlayerTeamColor   7
#define pin_laser               4
#define pin_fireButton          A7
#define pin_laserSensor         5
#define pin_RXESP               2 //13
#define pin_TXESP               3 //12
#define pin_IDOx1               8
#define pin_IDOx2               9
#define pin_IDOx3               10
#define pin_IDOx4               11
#define pin_IDOx5               12
#define pin_audioOut            6
//#define pin_IDOx6               -1

#define delay_playerAction    1 // ms
#define delay_laserAction     1// ms 6
#define delay_laserAction__laserSensorActionDivisionTime 1 // ms 3
#define delay_ESPCOMAction    5 // ms
#define delay_audioFrameAction    250 // ms
#define delay_flashKill       100 // ms
#define time_playerDead_default      4000 // ms  x  playerAction
#define offset_playerDeadCmd  250
#define serial_baudrate_ESP   9600
#define totalNCheck_LaserId   2

#define enableFireButtonSeuil false
#define fireButtonSensitive   512 //512  

#define enableFireButtonDelta true
#define fireButtonSensitiveDelta   20 // 
#define fireButtonSensitiveSeuil   210 //  

#define fireButtonDirectionHigh   true
#define startIdPlayerOffset   10
#define maxLedNeoPixel        255
#define lightnessNeoPixel     0.17  //float 0-1

void initPlayer();
void framePlayer();
void colorPlayer(Color);
void colorPlayerNone();
void blinkPLAYER();
void blinkLASER();
void blinkLASERSENSOR();
void blinkESPCOM();
void blinkPLAYERISDEAD();
void blinkAUDIOFRAME();
void setPlayerId(int);
void isReady();
void isNotReady();
void fireButtonEvents(int);
void fireButton_GoingON();
void fireButton_GoingOFF();
void fireButton_IsON();
void fireButton_IsOFF();
int arround(int, int);
void isKilled(int);
void commandESP(char);
void setIdByPins();
void play();

void playGorilla();
void playLaserSound();

#endif
