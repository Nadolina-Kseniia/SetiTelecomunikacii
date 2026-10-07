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

// ---------- ЕДИНСТВЕННЫЙ декодер ----------
void DECODE(void*) {
    while (!COMPLETED) {
        // 1) Ждём начало очередного блока: LINE стал ненулевым
        while (!COMPLETED && LINE == 0) Sleep(1);
        if (COMPLETED) break;

        DWORD t0 = GetTickCount();
        int bitIndex = 0;

        // 2) Пока линия активна — сэмплируем биты в СЕРЕДИНЕ интервалов
        while (!COMPLETED && bitIndex < 30) {
            DWORD target = t0
                + (DWORD)bitIndex * BIT_PERIOD
                + BIT_PERIOD / 2;      // +50 мс — запас от границ
            while (!COMPLETED && GetTickCount() < target) Sleep(1);
            if (COMPLETED) break;

            int val = LINE;
            if (val == 0) break;                // конец блока — пауза

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

// ---------- main ----------
int main() {
    timeBeginPeriod(1);                          // точный Sleep(1)

    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand((unsigned)time(NULL));

    cout << "=== Лабораторная работа №2. Задание 5.2 ===\n";

    _beginthread(DECODE, 0, NULL);               // ОДИН декодер на всё
    Sleep(315);

    cout << "\n--- Передача CODE1 ---\n";
    _beginthread(CODE1, 0, NULL);
    Sleep(2000);

    cout << "\n--- Передача CODE2 (задержка 250 мс) ---\n";
    Sleep(250);
    _beginthread(CODE2, 0, NULL);
    Sleep(2000);

    cout << "\n--- Передача CODE3 (задержка 250 мс) ---\n";
    Sleep(250);
    _beginthread(CODE3, 0, NULL);
    Sleep(2000);

    COMPLETED = true;
    Sleep(500);

    cout << "\n\n=== Результаты ===\n";
    cout << "Передано (" << sentCount << " бит): ";
    for (int i = 0; i < sentCount; i++) cout << SENT[i];
    cout << "\nПринято  (" << receivedCount << " бит): ";
    for (int i = 0; i < receivedCount; i++) cout << RECEIVED[i];
    cout << "\n";

    bool ok = (sentCount == receivedCount);
    if (ok)
        for (int i = 0; i < sentCount; i++)
            if (SENT[i] != RECEIVED[i]) { ok = false; break; }

    cout << (ok ? "Передача успешна!\n" : "Обнаружены искажения!\n");

    timeEndPeriod(1);
    return 0;
}