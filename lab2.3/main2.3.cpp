#include <iostream>
#include <cstdlib>
#include <process.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
using namespace std;

volatile bool COMPLETED = false;
volatile int  LINE = 0;

// Длинная серия одинаковых битов (20 нулей)
int MESS_LONG[20] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

int SENT[40];
int RECEIVED[40];
volatile int receivedCount = 0;
volatile int sentCount = 0;

const int BIT_PERIOD = 100;

// ---------- Передатчик ----------
void CODE_LONG(void*) {
    for (int i = 0; i < 20; i++) {
        SENT[sentCount++] = MESS_LONG[i];
        LINE = MESS_LONG[i] + 1;      // 0 -> 1
        Sleep(BIT_PERIOD);
    }
    LINE = 0;
    _endthread();
}

// ---------- Декодер (один на весь сеанс) ----------
void DECODE(void*) {
    while (!COMPLETED) {
        // Ждём фронт 0 -> не-0 (начало блока)
        while (!COMPLETED && LINE == 0) Sleep(1);
        if (COMPLETED) break;

        DWORD t0 = GetTickCount();
        int bitIndex = 0;

        while (!COMPLETED && bitIndex < 40) {
            // Сэмпл в СЕРЕДИНЕ i-го интервала
            DWORD target = t0 + (DWORD)bitIndex * BIT_PERIOD + BIT_PERIOD / 2;
            while (!COMPLETED && GetTickCount() < target) Sleep(1);
            if (COMPLETED) break;

            int val = LINE;
            if (val == 0) break;      // конец блока

            if (val == 2) { cout << "\nGotBit 1"; if (receivedCount < 40) RECEIVED[receivedCount++] = 1; }
            else if (val == 1) { cout << "\nGotBit 0"; if (receivedCount < 40) RECEIVED[receivedCount++] = 0; }
            bitIndex++;
        }
    }
    _endthread();
}

// ---------- main ----------
int main() {
    // ВНИМАНИЕ: timeBeginPeriod НЕ вызываем — это и есть источник дрейфа
    // Передатчик шлёт Sleep(100), реально ~100-115 мс.
    // Приёмник считает, что бит = ровно 100 мс. Дрейф копится.
    // Если добавить timeBeginPeriod(1) — дрейф исчезнет, и 20/20.

    setlocale(LC_ALL, "ru_RU.UTF-8");

    cout << "=== 5.4. Длинная серия нулей (20 бит) ===\n";
    cout << "BIT_PERIOD = " << BIT_PERIOD
        << " мс, timeBeginPeriod ВЫКЛ (имитируем неточные часы)\n";

    _beginthread(DECODE, 0, NULL);
    Sleep(315);

    _beginthread(CODE_LONG, 0, NULL);
    Sleep(3500);              // 20*100 = 2000 + большой запас

    COMPLETED = true;
    Sleep(300);

    cout << "\n\nПередано (" << sentCount << " бит): ";
    for (int i = 0; i < sentCount; i++) cout << SENT[i];
    cout << "\nПринято  (" << receivedCount << " бит): ";
    for (int i = 0; i < receivedCount; i++) cout << RECEIVED[i];
    cout << "\n";

    bool ok = (sentCount == receivedCount);
    if (ok)
        for (int i = 0; i < sentCount; i++)
            if (SENT[i] != RECEIVED[i]) { ok = false; break; }

    cout << (ok ? "Передача успешна!\n"
        : "РАССИНХРОНИЗАЦИЯ: счётчики бит разошлись!\n");
    return 0;
}