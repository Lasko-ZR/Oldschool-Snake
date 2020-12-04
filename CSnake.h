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
    void reset(SCoord start_pos);                   // "��������" �쳿
    void draw(CScreen& scr);                        // �������� ����������� �쳿 �� �����
    void move(const SCoord& delta, CScreen& scr);   // ����������� �쳿 �� ������ �������� 
    void grow(const SCoord& pos, int growbits);     // ��������� ������� �쳿
    bool into(const SCoord& pos);                   // �������� ��������� ��������� � ��� �쳿
    SCoord head();                                  // ����� ������� ���������� ������ �쳿
    int size();                                     // ����� ������� ������� �쳿 

private:
    CoordVector worm;       // ������ ��������� �������� ��� �쳿 
    char head_mark;         // ������, ���� ������������� ������ �쳿
    unsigned int drawn;     // ������� ������������� ��� �쳿
};

#endif // __CSNAKE_H__
