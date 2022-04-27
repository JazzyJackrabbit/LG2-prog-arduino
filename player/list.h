
#ifndef LIST_H
#define LIST_H

#include <Arduino.h>

class List {
public:
    byte length;
    char data[128];
    void append(char item) {
        if (length < 16) data[length++] = item;
    }
    void remove(byte index) {
        if (index >= length) return;
        memmove(&data[index], &data[index+1], length - index - 1);
        length--;
    }
};

#endif
