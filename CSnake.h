#pragma once

#ifndef __CSNAKE_H__
#define __CSNAKE_H__

#include <vector>

#include "SCoord.h"
#include "CScreen.h"

using namespace std;

typedef vector<SCoord> CoordVector;

class CSnake {
public:
    CSnake();
    void reset(SCoord start_pos);                   // "скидання" змії
    void draw(CScreen& scr);                        // первинне відмалювання змії на екрані
    void move(const SCoord& delta, CScreen& scr);   // пересування змії на приріст кординат 
    void grow(const SCoord& pos, int growbits);     // збільшення довжини змії
    bool into(const SCoord& pos);                   // перевірка попадання координат в тіло змії
    SCoord head();                                  // метод повертає координати голови змії
    int size();                                     // метод повертає довжину змії 

private:
    CoordVector worm;       // вектор координат сигментів тіла змії 
    char head_mark;         // символ, яким відмальовується голова змії
    unsigned int drawn;     // довжина відмальованого тіла змії
};

#endif // __CSNAKE_H__
