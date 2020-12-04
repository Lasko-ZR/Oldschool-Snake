
#include "CGame.h"

#include <iostream>
#include <cstring>
#include <conio.h>

#pragma warning(disable:4996) 

 // форматний рядок для форматування результату гри
const char* recordFormatStr = "%-15s  %9.4f  %4u  %7.2f  %s";

SRecord::SRecord() {
    name[0] = '\0';
    rating = 0.0;
    length = 0;
    game_time = 0;
    date = static_cast<time_t>(0);
}

void SRecord::as_string(char* buffer) {
    sprintf(buffer, recordFormatStr, name, rating, length, game_time, ctime(&date));
}

ostream& operator << (ostream& os, const SRecord& rec) {
    os
        << rec.rating << ' '
        << rec.length << ' '
        << rec.game_time << ' '
        << rec.date << ' '
        << rec.name << endl;
    return os;
}

istream& operator >> (istream& is, SRecord& rec) {
    is
        >> rec.rating
        >> rec.length
        >> rec.game_time
        >> rec.date;
    is.ignore(1);
    is.getline(&rec.name[0], 16);
    return is;
}

// функція порівняння результатів по рейтингу.
// Необхідна для роботи qsort () для сортування по спаданню
int rec_compare(const void* _op1, const void* _op2) {
    const SRecord* op1 = reinterpret_cast<const SRecord*>(_op1);
    const SRecord* op2 = reinterpret_cast<const SRecord*>(_op2);
    return static_cast<int>(op2->rating - op1->rating);
}

/*----- кінець структури SRecord -------------------------------------*/

// очистка буферу клавіатури
void clearkeys() {
    while (_kbhit())
        _getch();
}

// Конструктор
// _scr - об'єкт, який здійснює вивід на консоль
// _width - ширина ігрового поля (x)
// _height - висота ігрового поля (y)
// _latency - затримка між зміною позиції в мілісекундах

CGame::CGame(CScreen& _scr, int _width, int _height, int _latency) :
    width(_width), height(_height), latency(_latency), scr(_scr) {

    srand(static_cast<unsigned int>(time(NULL)));

    duration_game = 0;
    rating = 0.0;

    // ініціалізація таблиці команд управління грою
    cmd_table[0] = CmdPair(27, CMD_EXIT);   // евакуаційний ключ
    cmd_table[1] = CmdPair('K', CMD_LEFT);  // стрілка вліво
    cmd_table[2] = CmdPair('M', CMD_RIGHT); // стрілка вправо
    cmd_table[3] = CmdPair('H', CMD_UP);    // стрілка вверх
    cmd_table[4] = CmdPair('P', CMD_DOWN);  // стрілка вниз
}

CGame::Command CGame::get_command() {
    int ch;

    ch = _getch();
    if (ch == 0 || ch == 0xe0) {
        ch = _getch();
    }

    for (int i = 0; i < 5; i++) {
        if (cmd_table[i].first == ch) {
            return cmd_table[i].second;
        }
    }
    return CMD_NOCOMMAND;
}

// Координата їжі обчислюється випадковим чином.
// Обмеження: координата не повинна потрапляти в тіло змії.
SCoord CGame::make_food() {
    SCoord food;
    do {
        food.x = rand() % (width - 2) + 1;
        food.y = rand() % (height - 2) + 1;
    } while (snake.into(food));

    return food;
}

const char BORDER = '#';    // символ для рисування рамки ігрового поля

void CGame::draw_field() {

    scr.cls();

    for (int y = 0; y < height; y++) {
        if (y == 0 || y == height - 1) {
            for (int x = 0; x < width; x++)
                scr.pos(x, y, BORDER);
        }
        else {
            scr.pos(0, y, BORDER);
            scr.pos(width - 1, y, BORDER);
        }
    }
    scr.pos(0, height);
    _cprintf("Довжина: **** Рейтинг: ****. **** (****. ****) Час: ****. **");
}

void CGame::print_stat() {
    scr.pos(8, height);
    _cprintf("%04u", snake.size());
    scr.pos(22, height);
    _cprintf("%09.4f", rating);
    scr.pos(33, height);
    _cprintf("%09.4f", rating_i);
    scr.pos(51, height);
    _cprintf("%07.2f", duration_game);
}

void CGame::top10_table() {
    scr.cls();
    char buf[80];

    scr.pos_str(width / 2 - 12, 2, "***** T O P    1 0 *****");
    scr.pos_str(5, 4, "Ім'я              Рейтинг    Довжина  Час   Дата");

    for (int i = 0; i < 10; i++) {
        ttop10[i].as_string(buf);
        scr.pos_str(5, 5 + i, buf);
    }
}

void CGame::top10(bool after_game) {

    char buf[80];
    char buf_encoded[NAMELENGTH];

    top10_table();      // показати таблицю 10 кращих результатів

    time_t date = time(NULL);
    if (after_game) {
        // якщо гра була зіграна, то показати поточний результат
        scr.pos(5, 16);
        _cprintf(recordFormatStr, "Ваш результат", rating, snake.size(), duration_game, ctime(&date));
    }

    if (rating > ttop10[9].rating) {    // якщо рейтинг гри більше, ніж менший з 10 кращих ...
         // запитати ім'я гравця
        scr.pos_str(5, 20, "Ваше ім'я: _");
        scr.pos(16, 20);
        cin.getline(&buf[0], NAMELENGTH);
        clearkeys();
        OemToCharBuff(buf, buf_encoded, static_cast<DWORD>(NAMELENGTH));
        // замінити останній запис в таблиці 10 кращих результатів
        strcpy(ttop10[9].name, buf_encoded);
        ttop10[9].date = date;
        ttop10[9].game_time = duration_game;
        ttop10[9].length = snake.size();
        ttop10[9].rating = rating;
        // впорядкувати результати по спаданні
        qsort(ttop10, 10, sizeof(SRecord), rec_compare);
        // оновити таблицю на екрані
        top10_table();

        // оновити файл з 10 кращими результатами
        write_top10();
    }
}

void CGame::pak(int y) {
    scr.pos_str(width / 2 - 15, y, "Натисніть будь-яку клавішу, щоб продовжити...");
    _getch();
    clearkeys();
}

bool CGame::once_more() {
    scr.pos_str(width / 2 - 12, height - 3, "Щ Е   Р А З ?");

    int ch = _getch();
    clearkeys();
    if (ch == 'N' || ch == 'n' || ch == 27)
        return false;
    return true;
}

const char* top10_filename = "snake.dat";   // ім'я файлу для зберігання 10 кращих результатів

void CGame::read_top10() {
    ifstream fin(top10_filename);
    if (fin) {
        for (int i = 0; i < 10; i++)
            fin >> ttop10[i];
    }
    fin.close();
}

void CGame::write_top10() {
    ofstream fout(top10_filename);
    if (fout) {
        for (int i = 0; i < 10; i++)
            fout << ttop10[i];
    }
    fout.close();
}

const char* ver_number = "v 1.1";
const char* copyright = "(c) Cranium, 2014.";

void CGame::logo() {
    scr.pos_str(width / 2 - 9, 10, "O l d s c h o o l");
    scr.pos_str(width / 2 - 7, 12, "S  N  A  K  E");
    scr.pos_str(width / 2 - 3, 16, ver_number);
    scr.pos_str(width / 2 - 9, height, copyright);
    pak(22);
}

void CGame::goodbye() {
    scr.cls();
    _cprintf("Oldschool Snake %s\n%s\n", ver_number, copyright);
}

const char FOOD = '$';      // символ для виводу їжі

void CGame::game_loop() {

    duration_game = 0;
    rating = rating_i = 0.0;

    draw_field();           // намалювати ігрове поле

    snake.reset(SCoord(width / 2, height / 2));     // встановити змію: довжина 2,
                                                    // положення - в середині ігрового поля,
                                                    // напрямок - вліво
    Command cmd = CMD_NOCOMMAND;
    State stt = STATE_OK;
    // delta  містить приріст координат (dx, dy) для кожного переміщення змії по полю
    SCoord delta(-1, 0);                // початковий рух - вліво
    SCoord food = make_food();          // обчислити координати їжі
    scr.pos(food.x, food.y, FOOD);      // вивести їжу на екран

    snake.draw(scr);                    // первинне малювання змії

    print_stat();                       // вивести початкову статистику гри

    clock_t time1, time2, duration;
    time1 = clock();

    do {

        if (_kbhit())                   // якщо в буфері клавіатури є інформація,
            cmd = get_command();        // то приняти команду

        // обробка команд
        switch (cmd) {
        case CMD_LEFT:
            delta = SCoord(-1, 0);
            break;
        case CMD_RIGHT:
            delta = SCoord(1, 0);
            break;
        case CMD_UP:
            delta = SCoord(0, -1);
            break;
        case CMD_DOWN:
            delta = SCoord(0, 1);
            break;
        case CMD_EXIT:
            stt = STATE_EXIT;
        default:
            break;
        };

        SCoord hd = snake.head();       // координата голови змії
        hd += delta;                    // координата голови змії після збільшення (наступна позиція)
        // якщо голова змії зіткнулася з кордоном поля або з тілом змії, то змія вмирає
        if (hd.x == 0 || hd.x == width - 1 || hd.y == 0 || hd.y == height - 1 || snake.into(hd))
            stt = STATE_DIED;

        if (stt == STATE_OK) {          // якщо змія ще жива, то
            snake.move(delta, scr);     // зсунути змію на delta

            if (snake.head() == food) { // якщо координата голови змії збігається з координатою їжі, то
                snake.grow(food, 3);    // збільшити довжину змії
                food = make_food();     // вирахувати координати нової їжі
                scr.pos(food.x, food.y, FOOD); // вивести їжу на екран

                // Обчислення часу гри, часткового і загального рейтингу.
                // Частковий рейтинг обчислюється як довжина змії, поділена на час в секундах,
                // витрачений на підхід до їжі (час від поїдання попередньої їжі до поїдання наступної).
                // Таким чином, чим частіше змія їсть і чим вона довша, тим вище частковий рейтинг.
                time2 = clock();
                duration = time2 - time1;
                duration_game += static_cast<double>(duration) / CLOCKS_PER_SEC;
                rating_i = static_cast<double>(snake.size()) / duration * CLOCKS_PER_SEC;
                rating += rating_i;     // загальний рейтинг - сума часткових рейтингів за гру
                time1 = time2;

                print_stat();           // вивід поточної статистики гри
            }

            Sleep(latency);             // затримка перед наступною зміною позиції
        }

    } while (stt == STATE_OK);          // граємо, поки змія жива

    scr.pos_str(width / 2 - 8, 10, " Г Р А   З А В Е Р Ш Е Н А ");
    clearkeys();
    _getch();
    clearkeys();
}
