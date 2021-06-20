#ifndef CALCULATOR_H
#define CALCULATOR_H
#include <Arduino.h>
class Calculator
{
public:
    Calculator() {}
    int add(int a, int b);
    int minus(int a, int b);
};
#endif