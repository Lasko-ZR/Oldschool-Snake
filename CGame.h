#pragma once

#ifndef __CGAME_H__
#define __CGAME_H__

#include <time.h>
#include <fstream>
#include <utility>

#include "CScreen.h"
#include "CSnake.h"
#include "SCoord.h"

using namespace std;

const int NAMELENGTH = 16;      // розмір буфера для імені гравця

// Структура для зберігання результатів гри

struct SRecord {
    char name[NAMELENGTH];      // Ім'я гравця
    double rating;              // рейтинг
    int length;                 // довжинна змійки
    double game_time;           // час гри
    time_t date;                // дата і час закінчення гри

    SRecord();
    void as_string(char* buffer);   // відформатована стрічка результтів
};

class CGame {
public:
    CGame(CScreen& _scr, int _width = 80, int _height = 24, int _latency = 100);
    void game_loop();           // головний цикл гри
    void top10(bool after_game); // работа з таблицею 10 кращих результатів
    bool once_more();           //  Вивід запиту та прийом відповіді від гравця
    void pak(int y);            // "Натисніть будь-яку клавішу, щоб продовжити..."
    void read_top10();          // Читання з файлу таблиці 10 кращих результатів
    void write_top10();         // запис в файл таблиці 10 кращих результатів
    void logo();                // вивід заставки гри
    void goodbye();             // вивід копірайту після закінчення гри

private:
    enum Command { CMD_NOCOMMAND = 0, CMD_EXIT, CMD_LEFT, CMD_RIGHT, CMD_UP, CMD_DOWN };
    enum State { STATE_OK, STATE_EXIT, STATE_DIED };

    typedef pair<int, Command> CmdPair;

    int width, height;      // ширина и висота поля гри
    int latency;            // затримка між змінами позиції в мілісекундах
    CScreen scr;            // підсистема візуалізації
    CSnake snake;           // змійка
    double duration_game;   // тривалість грм
    double rating, rating_i; // частковий і підсумковий рейтинг

    SRecord ttop10[10];     // таблиця 10 кращих результатів

    CmdPair cmd_table[5];   // таблиця команд управління грою

    void draw_field();      // промальовування ігрового поля
    SCoord make_food();     // обчислення позиції для їжі
    void print_stat();      // вивід поточної статистики нижче ігрового поля
    Command get_command();  // прийом команди з клавіатури
    void top10_table();     // вивід таблиці 10 кращих результатів
};

#endif // __CGAME_H__
