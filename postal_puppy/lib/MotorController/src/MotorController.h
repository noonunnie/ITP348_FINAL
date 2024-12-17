#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include "Particle.h"  // Include Particle library

// Motor pin definitions
const int PWMA = A0;
const int AIN2 = D7;
const int AIN1 = D6;
const int BIN1 = D5;
const int BIN2 = D4;
const int PWMB = A1;

// Function declarations
void forward();
void backward();
void left();
void right();
void stop();
void clockwise();
void counterwise();

#endif  // MOTORCONTROLLER_H
