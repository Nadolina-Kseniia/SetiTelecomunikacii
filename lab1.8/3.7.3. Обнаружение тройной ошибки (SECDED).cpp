#include <iostream>
#include <vector>
#include <clocale>
using namespace std;

vector<int> encode(const vector<int>& d) {
    vector<int> h(9);              // позиции 1..8
    h[3] = d[0]; h[5] = d[1]; h[6] = d[2]; h[7] = d[3];
    h[1] = h[3] ^ h[5] ^ h[7];
    h[2] = h[3] ^ h[6] ^ h[7];
    h[4] = h[5] ^ h[6] ^ h[7];
    h[8] = h[1] ^ h[2] ^ h[3] ^ h[4] ^ h[5] ^ h[6] ^ h[7];   // общий бит чётности
    return h;
}

void printCode(const vector<int>& h, const char* label) {
    cout << label;
    for (int i = 1; i <= 8; i++) cout << h[i];
    cout << "\n";
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    vector<int> data(4);
    cout << "Введите 4 информационных бита: ";
    for (int i = 0; i < 4; i++) cin >> data[i];

    vector<int> h = encode(data);
    printCode(h, "Закодировано (7,4)+parity: ");

    int n;
    cout << "Сколько ошибок внести (1..3): ";
    cin >> n;
    for (int e = 0; e < n; e++) {
        int pos;
        cout << "Позиция ошибки " << (e + 1) << " (1..8): ";
        cin >> pos;
        if (pos >= 1 && pos <= 8) h[pos] ^= 1;
    }
    printCode(h, "Принято:                   ");

    int s1 = h[1] ^ h[3] ^ h[5] ^ h[7];
    int s2 = h[2] ^ h[3] ^ h[6] ^ h[7];
    int s4 = h[4] ^ h[5] ^ h[6] ^ h[7];
    int syndrome = s1 + 2 * s2 + 4 * s4;
    int overall = h[1] ^ h[2] ^ h[3] ^ h[4] ^ h[5] ^ h[6] ^ h[7] ^ h[8];

    cout << "\nСиндром c = " << syndrome
        << ", общая чётность = " << (overall ? "нарушена" : "верна") << "\n";

    if (syndrome == 0 && overall == 0) {
        cout << "→ Ошибок нет.\n";
    }
    else if (syndrome != 0 && overall == 1) {
        cout << "→ Одиночная ошибка (или тройная). Пытаемся исправить как одиночную.\n";
        cout << "  Инвертируем бит " << syndrome << ".\n";
        h[syndrome] ^= 1;
        vector<int> restored = { h[3], h[5], h[6], h[7] };
        cout << "  Результат: ";
        for (int b : restored) cout << b;
        cout << (restored == data
            ? "  ✓ (была одиночная — исправлено)\n"
            : "  ✗ (была тройная — данные искажены!)\n");
    }
    else if (syndrome != 0 && overall == 0) {
        cout << "→ Двойная ошибка. Исправление невозможно.\n";
    }
    else {
        cout << "→ Ошибка в бите общей чётности (позиция 8). Исправляем.\n";
        h[8] ^= 1;
    }

    return 0;
}