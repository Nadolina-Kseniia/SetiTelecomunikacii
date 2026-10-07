#include <iostream>
#include <vector>
#include <clocale>
using namespace std;

vector<int> hamming74_encode(const vector<int>& d) {
    vector<int> h(8);              // позиции 1..7
    h[3] = d[0];
    h[5] = d[1];
    h[6] = d[2];
    h[7] = d[3];
    h[1] = h[3] ^ h[5] ^ h[7];     // p1
    h[2] = h[3] ^ h[6] ^ h[7];     // p2
    h[4] = h[5] ^ h[6] ^ h[7];     // p4
    return h;
}

int hamming74_decode(vector<int>& h, vector<int>& outData) {
    int s1 = h[1] ^ h[3] ^ h[5] ^ h[7];
    int s2 = h[2] ^ h[3] ^ h[6] ^ h[7];
    int s4 = h[4] ^ h[5] ^ h[6] ^ h[7];
    int syndrome = s1 + 2 * s2 + 4 * s4;

    outData = { h[3], h[5], h[6], h[7] };

    if (syndrome == 0) return 0;                     // ошибок нет
    if (syndrome >= 1 && syndrome <= 7) {            // одиночная
        h[syndrome] ^= 1;
        outData = { h[3], h[5], h[6], h[7] };
        return 1;
    }
    return 2;                                        // нештатная ситуация
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    vector<int> data(4);
    cout << "Введите 4 информационных бита (0/1): ";
    for (int i = 0; i < 4; i++) cin >> data[i];

    vector<int> h = hamming74_encode(data);
    cout << "Код (7,4): ";
    for (int i = 1; i <= 7; i++) cout << h[i];
    cout << "\n";

    int pos;
    cout << "Позиция ошибки (1..7, 0 — без ошибки): ";
    cin >> pos;
    if (pos >= 1 && pos <= 7) h[pos] ^= 1;

    cout << "Принято:   ";
    for (int i = 1; i <= 7; i++) cout << h[i];
    cout << "\n";

    vector<int> restored;
    int st = hamming74_decode(h, restored);

    cout << "\nСиндром → ";
    if (st == 0) cout << "ошибок нет.\n";
    else if (st == 1) cout << "одиночная ошибка исправлена.\n";
    else cout << "ошибка в служебной части.\n";

    cout << "Восстановленная информация: ";
    for (int b : restored) cout << b;
    cout << "\n";

    cout << (restored == data ? "✓ Совпадает с исходной\n"
        : "✗ Отличается от исходной\n");
    return 0;
}