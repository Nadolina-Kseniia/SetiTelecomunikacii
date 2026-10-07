#include <iostream>
#include <cstdlib>
#include <ctime>
#include <process.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
using namespace std;

volatile bool COMPLETED = false;
volatile bool SYNC_EVENT = false;   // взводится на каждом фронте LINE
volatile int  LINE = 0;

int MESS1[10] = { 1,1,0,0,1,1,0,1,1,1 };   // 1100110111
int MESS2[10] = { 1,0,0,0,0,0,1,1,1,0 };   // 1000001110
int MESS3[10] = { 0,1,0,0,1,1,1,0,1,1 };   // 0100111011

int SENT[30];
int RECEIVED[30];
volatile int receivedCount = 0;
volatile int sentCount = 0;

const int BIT_PERIOD = 100;

// ---------- Передатчики ----------
void CODE1(void*) {
    for (int i = 0; i < 10; i++) {
        SENT[sentCount++] = MESS1[i];
        LINE = MESS1[i] + 1;        // 1->2, 0->1
        Sleep(BIT_PERIOD);
    }
    LINE = 0;
    _endthread();
}
void CODE2(void*) {
    for (int i = 0; i < 10; i++) {
        SENT[sentCount++] = MESS2[i];
        LINE = MESS2[i] + 1;
        Sleep(BIT_PERIOD);
    }
    LINE = 0;
    _endthread();
}
void CODE3(void*) {
    for (int i = 0; i < 10; i++) {
        SENT[sentCount++] = MESS3[i];
        LINE = MESS3[i] + 1;
        Sleep(BIT_PERIOD);
    }
    LINE = 0;
    _endthread();
}

// ---------- Слушатель линии: фиксирует фронты ----------
void LISTENER(void*) {
    int prev = LINE;
    while (!COMPLETED) {
        int cur = LINE;
        if (cur != prev) {
            prev = cur;
            SYNC_EVENT = true;      // сообщаем декодеру о новом бите
        }
        Sleep(2);
    }
    _endthread();
}

// ---------- ЕДИНСТВЕННЫЙ декодер ----------
void DECODE(void*) {
    // Ждём первый фронт (начало передачи)
    while (!SYNC_EVENT && !COMPLETED) Sleep(1);
    if (COMPLETED) { _endthread(); return; }
    SYNC_EVENT = false;

    DWORD t0 = GetTickCount();
    int bitIndex = 0;

    while (!COMPLETED && bitIndex < 30) {
        // ВАЖНО: сэмплируем в СЕРЕДИНЕ битового интервала, а не в начале.
        // + BIT_PERIOD/2 = 50 мс — гарантированный запас ±50 мс от границ.
        DWORD target = t0 + (DWORD)bitIndex * BIT_PERIOD + BIT_PERIOD / 2;
        while (!COMPLETED && GetTickCount() < target) Sleep(1);
        if (COMPLETED) break;

        int val = LINE;
        if (val == 2) {
            cout << "\nGotBit 1";
            RECEIVED[receivedCount++] = 1;
        }
        else if (val == 1) {
            cout << "\nGotBit 0";
            RECEIVED[receivedCount++] = 0;
        }
        else {
            // LINE == 0 — передача закончилась, бит не пишем
            break;
        }
        bitIndex++;
    }
    _endthread();
}

// ---------- main ----------
int main() {
    timeBeginPeriod(1);                          // ← включает точный таймер

    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand((unsigned)time(NULL));

    cout << "=== Лабораторная работа №2. Задание 5.1 ===\n";

    _beginthread(LISTENER, 0, NULL);
    _beginthread(DECODE, 0, NULL);
    Sleep(315);

    _beginthread(CODE1, 0, NULL);
    Sleep(2000);

    COMPLETED = true;
    Sleep(500);

    cout << "\n\nПереданная последовательность: ";
    for (int i = 0; i < sentCount; i++) cout << SENT[i];
    cout << "\nПринятая последовательность:  ";
    for (int i = 0; i < receivedCount; i++) cout << RECEIVED[i];
    cout << "\n";

    bool ok = (sentCount == receivedCount);
    if (ok)
        for (int i = 0; i < sentCount; i++)
            if (SENT[i] != RECEIVED[i]) { ok = false; break; }

    cout << (ok ? "Передача успешна!\n" : "Обнаружены искажения!\n");

    timeEndPeriod(1);                            // ← выключаем
    return 0;
}