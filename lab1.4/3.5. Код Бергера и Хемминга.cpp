#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <clocale>
using namespace std;

// ================== КОД БЕРГЕРА ==================
vector<int> bergerEncode(const vector<int>& data) {
    int i = data.size();
    int k = 0;
    while ((1 << k) < i + 1) k++;

    int ones = 0;
    for (int b : data) ones += b;

    vector<int> cnt(k);
    for (int j = 0; j < k; j++) cnt[k - 1 - j] = (ones >> j) & 1;
    for (int& b : cnt) b ^= 1;

    vector<int> code = data;
    code.insert(code.end(), cnt.begin(), cnt.end());
    return code;
}

int bergerDecode(const vector<int>& received, int infoBits, vector<int>& outData) {
    vector<int> data(received.begin(), received.begin() + infoBits);
    vector<int> kBits(received.begin() + infoBits, received.end());
    int k = kBits.size();

    int ones = 0;
    for (int b : data) ones += b;

    vector<int> expected(k);
    for (int j = 0; j < k; j++) expected[k - 1 - j] = (ones >> j) & 1;
    for (int& b : expected) b ^= 1;

    outData = data;
    return (expected == kBits) ? 0 : 1;   // 0 — ok, 1 — обнаружено
}

// ================== КОД ХЕММИНГА (7,4) + SECDED ==================
// 4 информационных бита -> 8 бит (позиции 1..8)
vector<int> hammingEncode(const vector<int>& d) {
    vector<int> h(9);
    h[3] = d[0];
    h[5] = d[1];
    h[6] = d[2];
    h[7] = d[3];

    h[1] = h[3] ^ h[5] ^ h[7];
    h[2] = h[3] ^ h[6] ^ h[7];
    h[4] = h[5] ^ h[6] ^ h[7];
    h[8] = h[1] ^ h[2] ^ h[3] ^ h[4] ^ h[5] ^ h[6] ^ h[7];   // overall parity

    return h;
}

// Декодирование:
//   0 — ошибок нет
//   1 — одиночная ошибка исправлена
//   2 — двойная ошибка, исправить нельзя
//   3 — ошибка в общем бите чётности (исправлена)
int hammingDecode(vector<int>& h, vector<int>& outData) {
    int s1 = h[1] ^ h[3] ^ h[5] ^ h[7];
    int s2 = h[2] ^ h[3] ^ h[6] ^ h[7];
    int s4 = h[4] ^ h[5] ^ h[6] ^ h[7];
    int syndrome = s1 + 2 * s2 + 4 * s4;
    int overall = h[1] ^ h[2] ^ h[3] ^ h[4] ^ h[5] ^ h[6] ^ h[7] ^ h[8];

    int status;
    if (syndrome == 0 && overall == 0) {
        status = 0;
    }
    else if (syndrome != 0 && overall == 1) {
        if (syndrome >= 1 && syndrome <= 7) h[syndrome] ^= 1;
        status = 1;
    }
    else if (syndrome != 0 && overall == 0) {
        status = 2;
    }
    else {  // syndrome == 0, overall == 1
        h[8] ^= 1;
        status = 3;
    }
    outData = { h[3], h[5], h[6], h[7] };
    return status;
}

// ================== ОБЩИЙ ПРОГОН ==================
void runBergerExperiment(int infoBits, double Q, int N) {
    int correct = 0, detected = 0, undetected = 0;
    for (int t = 0; t < N; t++) {
        vector<int> d(infoBits);
        for (int i = 0; i < infoBits; i++) d[i] = rand() % 2;

        vector<int> c = bergerEncode(d);
        for (int& b : c)
            if ((double)rand() / RAND_MAX < Q) b ^= 1;

        vector<int> r;
        int st = bergerDecode(c, infoBits, r);
        if (st == 0) { if (r == d) correct++; else undetected++; }
        else detected++;
    }
    int k = 0; while ((1 << k) < infoBits + 1) k++;
    int totalBits = infoBits + k;

    cout << "\n===== Код Бергера (" << infoBits << "," << totalBits << ") =====\n";
    cout << "Пакет: " << totalBits << " бит, избыточность = "
        << fixed << setprecision(2)
        << 100.0 * k / totalBits << " %\n";
    cout << "Q = " << Q << ", N = " << N << "\n";
    cout << "  Верно передано:        " << correct
        << " (" << 100.0 * correct / N << " %)\n";
    cout << "  Обнаружено:            " << detected
        << " (" << 100.0 * detected / N << " %)\n";
    cout << "  НЕ обнаружено:         " << undetected
        << " (" << 100.0 * undetected / N << " %)\n";
    cout << "  Успешно (верно+обн.):  " << (correct + detected)
        << " (" << 100.0 * (correct + detected) / N << " %)\n";
}

void runHammingExperiment(double Q, int N) {
    // Работаем блоками по 4 информационных бита.
    // В 8-битном коде Хемминга (7,4)+SECDED 4 информационных бита.
    // Чтобы сравнить с Бергером (4, 7), используем 4-битные пакеты.
    int correct = 0, corrected = 0, detected = 0, undetected = 0;

    for (int t = 0; t < N; t++) {
        vector<int> d(4);
        for (int i = 0; i < 4; i++) d[i] = rand() % 2;

        vector<int> c = hammingEncode(d);
        for (int i = 1; i <= 8; i++)
            if ((double)rand() / RAND_MAX < Q) c[i] ^= 1;

        vector<int> r;
        int st = hammingDecode(c, r);

        if (st == 0) {
            if (r == d) correct++;
            else undetected++;
        }
        else if (st == 1 || st == 3) {
            // одиночная ошибка исправлена
            if (r == d) corrected++;
            else undetected++;        // исправлено неверно
        }
        else {  // st == 2 — двойная
            detected++;
        }
    }

    cout << "\n===== Код Хемминга (7,4)+SECDED (4, 8) =====\n";
    cout << "Пакет: 8 бит, избыточность = "
        << fixed << setprecision(2)
        << 100.0 * 4 / 8 << " %\n";
    cout << "Q = " << Q << ", N = " << N << "\n";
    cout << "  Верно передано (без ошибок):  " << correct
        << " (" << 100.0 * correct / N << " %)\n";
    cout << "  Исправлено (одиночных):       " << corrected
        << " (" << 100.0 * corrected / N << " %)\n";
    cout << "  Обнаружено (двойных):         " << detected
        << " (" << 100.0 * detected / N << " %)\n";
    cout << "  НЕ обнаружено / не исправлено:" << undetected
        << " (" << 100.0 * undetected / N << " %)\n";
    cout << "  Успешно (верно+испр.+обн.):   "
        << (correct + corrected + detected)
        << " (" << 100.0 * (correct + corrected + detected) / N << " %)\n";
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand(time(NULL));

    double Q = 0.1;
    int N = 100000;

    cout << "Сравнение кодов Бергера и Хемминга\n";
    cout << "Q = " << Q << ", N = " << N << "\n";

    // 4-битные пакеты — для сопоставления с Хеммингом (4,8)
    runBergerExperiment(4, Q, N);
    runHammingExperiment(Q, N);

    // Дополнительно — Бергер для 8 и 16 бит (чтобы была общая картина)
    runBergerExperiment(8, Q, N);
    runBergerExperiment(16, Q, N);

    return 0;
}