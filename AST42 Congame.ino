#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "Board.h"
int GateInput = A6;
int GateOut = 6;
int val1;
int gamespeed = A7;  // Speed of GOL
int val2;    // variable to read the value from the analog pin

Adafruit_8x8matrix matrix = Adafruit_8x8matrix();
void display_board(const Board & board) {
  for (size_t y = 0; y < board.size(); y++) {
    for (size_t x = 0; x < board.size(); x++) {
      if (board.get(x, y)) {
        matrix.drawPixel(x, y, LED_ON);
      } else {
        matrix.drawPixel(x, y, LED_OFF);
      }
    }
  }
  matrix.writeDisplay();
}
void next(const Board & src, Board & dest) {
  for (size_t y = 0; y < src.size(); y++) {
    for (size_t x = 0; x < src.size(); x++) {
      int neighbors = src.getModulus(x - 1, y + 1) + src.getModulus(x, y + 1) + src.getModulus(x + 1, y + 1) +
                      src.getModulus(x - 1, y)                            + src.getModulus(x + 1, y) +
                      src.getModulus(x - 1, y - 1) + src.getModulus(x, y - 1) + src.getModulus(x + 1, y - 1);
      bool live = true;
      if (src.get(x, y)) {
        if (neighbors < 2 || 3 < neighbors) {
          live = false;
        }
      } else {
        if (neighbors != 3) {
          live = false;
        }
      }
      dest.set(x, y, live);
    }
  }
}
void display_iter(const Board & src, Board & dest) {
  display_board(src);

  next(src, dest);
}
void randomize(Board & board) {
  for (size_t y = 0; y < board.size(); y++) {
    for (size_t x = 0; x < board.size(); x++) {
      board.set(x, y, random(2));
    }
  }
}
Board boardeven;
Board boardodd;
bool even = true;

void setup() {
  pinMode(GateInput, INPUT);
  pinMode(GateOut, OUTPUT);
  matrix.begin(0x70);  // pass in the address
  matrix.setBrightness(0);
  randomSeed(analogRead(0));
  randomize(boardeven);
  //boardeven.set(1, 0, true);
  //boardeven.set(2, 1, true);
  //boardeven.set(0, 2, true);
  //boardeven.set(1, 2, true);
  //boardeven.set(2, 2, true);
  even = true;

void loop() {
  {
    val1 = analogRead(GateInput);
    val1 = map(val1, 0, 1023, 0, 255);
    digitalWrite(GateOut, HIGH);
    delay(2);
    digitalWrite(GateOut, LOW);
  }
  {
    val2 = analogRead(gamespeed);            // reads the value of the potentiometer (value between 0 and 1023)
    val2 = map(val2, 0, 1023, 255, 0);   // scale it to use it with the servo (value between 0 and 180)
    delay(val2);

    if (even) {
      display_iter(boardeven, boardodd);
    } else {
      display_iter(boardodd, boardeven);
    }
    even = !even;
    if (boardeven == boardodd) {
      randomize(boardeven);
      even = true;
    }
  }
}
