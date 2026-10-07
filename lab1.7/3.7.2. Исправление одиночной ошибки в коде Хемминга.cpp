#include <iostream>
#include <vector>
#include <clocale>
using namespace std;

vector<int> encode(const vector<int>& d) {
    vector<int> h(8);
    h[3] = d[0]; h[5] = d[1]; h[6] = d[2]; h[7] = d[3];
    h[1] = h[3] ^ h[5] ^ h[7];
    h[2] = h[3] ^ h[6] ^ h[7];
    h[4] = h[5] ^ h[6] ^ h[7];
    return h;
}

int syndrome(const vector<int>& h) {
    int s1 = h[1] ^ h[3] ^ h[5] ^ h[7];
    int s2 = h[2] ^ h[3] ^ h[6] ^ h[7];
    int s4 = h[4] ^ h[5] ^ h[6] ^ h[7];
    return s1 + 2 * s2 + 4 * s4;
}

void printCode(const vector<int>& h, const char* label) {
    cout << label;
    for (int i = 1; i <= 7; i++) cout << h[i];
    cout << "\n";
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    vector<int> data(4);
    cout << "Введите 4 информационных бита: ";
    for (int i = 0; i < 4; i++) cin >> data[i];

    vector<int> code = encode(data);
    printCode(code, "Передано: ");

    // --- Одиночная ошибка ---
    int pos1;
    cout << "\n[Случай 1] Введите позицию ОДИНОЧНОЙ ошибки (1..7): ";
    cin >> pos1;
    vector<int> r1 = code;
    r1[pos1] ^= 1;
    printCode(r1, "Принято:  ");
    int c1 = syndrome(r1);
    cout << "Синдром c = " << c1 << "\n";
    if (c1 == 0) cout << "  -> ошибок нет\n";
    else {
        cout << "  -> одиночная ошибка в позиции " << c1 << "\n";
        r1[c1] ^= 1;
        vector<int> restored = { r1[3], r1[5], r1[6], r1[7] };
        cout << "  После исправления: ";
        for (int b : restored) cout << b;
        cout << (restored == data ? "  ✓ данные верны\n" : "  ✗ ошибка\n");
    }

    // --- Двойная ошибка ---
    int p1, p2;
    cout << "\n[Случай 2] Введите две позиции ДВОЙНОЙ ошибки: ";
    cin >> p1 >> p2;
    vector<int> r2 = code;
    r2[p1] ^= 1;
    r2[p2] ^= 1;
    printCode(r2, "Принято:  ");
    int c2 = syndrome(r2);
    cout << "Синдром c = " << c2
        << " = " << p1 << " XOR " << p2 << " = " << (p1 ^ p2) << "\n";
    cout << "  -> Это НЕ совпадает ни с одной из реальных позиций.\n";
    cout << "  -> Декодер попытается «исправить» бит " << c2
        << " и внесёт ДОПОЛНИТЕЛЬНУЮ ошибку.\n";

    return 0;
}