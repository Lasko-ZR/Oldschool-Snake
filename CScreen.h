#pragma once

#ifndef __CSCREEN_H__
#define __CSCREEN_H__

#include <windows.h>

 /*
 Клас виключення для класа CScreen
 */

class CSScreenException {
public:
    CSScreenException(int _err) : err(_err) {}
    const char* what();         // повертає рядок з описом помилки
    int err;                    // код помилки
};

/*
Клас CScreen містить системозалежні виклики для виведення на консоль.

Система координат:
     (0, 0) - лівий верхній кут екрану
     вісь X - горизонтально вправо
     вісь Y - вертикально вниз (позитивний напрямок)
*/

class CScreen {
public:
    CScreen();
    ~CScreen();
    void cursor_show(bool visible);                 // показати/скрити курсор
    void text_attr(WORD attr);                      // встановити колір тексту/фона
    void pos(int x, int y, char ch = 0);            // позиціонування курсору і
                                                    // виведення символу, якщо ch! = 0
    void pos_str(int x, int y, const char* str);    // позиціонування курсору
                                                    // и вивід рядка
    void cls();                                     // очищення екрану

private:
    HANDLE hConsoleOutput;
    CONSOLE_CURSOR_INFO oldCursorInfo, curCursorInfo;
    WORD oldTextAttr;
};

#endif // __CSCREEN_H__

