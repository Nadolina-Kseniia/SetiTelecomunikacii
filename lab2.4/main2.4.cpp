#include <iostream>
#include <cstdlib>
#include <ctime>
#include <process.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
using namespace std;

volatile bool COMPLETED = false;
volatile int  LINE = 0;

int MESS1[10] = { 1,1,0,1,0,0,0,1,1,1 };
int MESS2[10] = { 1,0,0,1,0,1,1,1,1,1 };
int MESS3[10] = { 0,0,0,0,1,1,1,1,1,0 };

int SENT[30];
int RECEIVED[30];
volatile int receivedCount = 0;
volatile int sentCount = 0;

const int BIT_PERIOD = 100;

// Вероятность ошибки в канале, % (задаётся в main)
volatile int NOISE_PERCENT = 10;

// ---------- Передатчики ----------
void CODE1(void*) {
    for (int i = 0; i < 10; i++) {
        SENT[sentCount++] = MESS1[i];
        LINE = MESS1[i] + 1;
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

// ---------- ЕДИНСТВЕННЫЙ декодер (как в 5.3/5.4) ----------
void DECODE(void*) {
    while (!COMPLETED) {
        while (!COMPLETED && LINE == 0) Sleep(1);
        if (COMPLETED) break;

        DWORD t0 = GetTickCount();
        int bitIndex = 0;

        while (!COMPLETED && bitIndex < 30) {
            DWORD target = t0 + (DWORD)bitIndex * BIT_PERIOD + BIT_PERIOD / 2;
            while (!COMPLETED && GetTickCount() < target) Sleep(1);
            if (COMPLETED) break;

            int val = LINE;
            if (val == 0) break;

            if (val == 2) {
                cout << "\nGotBit 1";
                if (receivedCount < 30) RECEIVED[receivedCount++] = 1;
            }
            else if (val == 1) {
                cout << "\nGotBit 0";
                if (receivedCount < 30) RECEIVED[receivedCount++] = 0;
            }
            bitIndex++;
        }
    }
    _endthread();
}

// ---------- ШУМ ----------
// Кратковременно инвертирует LINE с вероятностью NOISE_PERCENT %.
// Длительность помехи — 20 мс; этого достаточно, чтобы попасть
// в момент сэмплирования (середина бита) с некоторой вероятностью.
void NOISE(void*) {
    while (!COMPLETED) {
        if (LINE != 0 && (rand() % 100) < NOISE_PERCENT) {
            int saved = LINE;
            LINE = (saved == 2) ? 1 : 2;   // инверсия бита
            Sleep(20);
            // восстанавливаем только если передатчик за это время
            // не поменял LINE сам
            if (LINE == ((saved == 2) ? 1 : 2))
                LINE = saved;
        }
        Sleep(5);
    }
    _endthread();
}

// ---------- main ----------
int main() {
    timeBeginPeriod(1);              // точный Sleep — чтобы НЕ было дрейфа

    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand((unsigned)time(NULL));

    cout << "=== Задание 5.5. Зашумлённый канал ===\n";
    cout << "Вероятность ошибки в канале: " << NOISE_PERCENT << "% на бит\n\n";

    _beginthread(DECODE, 0, NULL);
    _beginthread(NOISE, 0, NULL);
    Sleep(315);

    _beginthread(CODE1, 0, NULL);
    Sleep(1500);

    COMPLETED = true;
    Sleep(300);

    cout << "\n\n=== Результаты ===\n";
    cout << "Передано (" << sentCount << "): ";
    for (int i = 0; i < sentCount; i++) cout << SENT[i];
    cout << "\nПринято  (" << receivedCount << "): ";
    for (int i = 0; i < receivedCount; i++) cout << RECEIVED[i];
    cout << "\n";

    int n = min(sentCount, receivedCount);
    int errors = 0;
    for (int i = 0; i < n; i++)
        if (SENT[i] != RECEIVED[i]) errors++;

    cout << "Ошибок: " << errors << " из " << n << "\n";
    cout << "Доля верных бит: "
        << (n ? 100.0 * (n - errors) / n : 0.0) << "%\n";

    bool ok = (sentCount == receivedCount && errors == 0);
    cout << (ok ? "Передача успешна!\n"
        : "Передача НЕ успешна — обнаружены искажения!\n");

    timeEndPeriod(1);
    return 0;
}