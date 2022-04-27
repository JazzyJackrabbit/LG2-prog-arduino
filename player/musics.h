
#ifndef MUSICS_H
#define MUSICS_H
#include <Arduino.h>
#include "pitches.h";
#include "list.h";
  
List melody_laser { .length = 17, .data = {
    0,1,  
    3450, 1,           3200, 1,        3050, 1,           2900, 1,
    2750, 1,          2600, 1,
    2150, 1,        2000, 1,          1850, 1,         1600, 1,
    1450, 1,           1200, 1,        1050, 1,           900, 1,
    750, 1,          600, 1,
     
    
    //NOTE_C4, 1,  NOTE_A3, 1,  NOTE_F3, 1,  NOTE_D3, 1,
    //NOTE_C3, 1,  0, 2,
}};

List melody_gorilla  { .length = 17, .data = {
  NOTE_C2,
  NOTE_C3, 
  NOTE_D3, 
  NOTE_E3, 
  NOTE_D3, 
  NOTE_C3, 
  NOTE_D3, 
  NOTE_E3,
  NOTE_E3,
  NOTE_C3, 
  NOTE_C3, 
  NOTE_C3, 
  NOTE_C3, 
}};

#endif
