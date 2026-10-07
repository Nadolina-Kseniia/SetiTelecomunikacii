#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <cmath>
#include <clocale>
using namespace std;

// 0 — верно, 1 — обнаружена, 2 — не обнаружена
int runOnePacket(int infoBits, double Q) {
    vector<int> data(infoBits);
    for (int i = 0; i < infoBits; i++) data[i] = rand() % 2;

    int parity = 0;
    for (int b : data) parity ^= b;
    data.push_back(parity);

    int errors = 0;
    for (int& b : data)
        if ((double)rand() / RAND_MAX < Q) { b ^= 1; errors++; }

    int sum = 0;
    for (int b : data) sum ^= b;

    if (sum == 1) return 1;                // чётность нарушена — обнаружено
    return (errors == 0) ? 0 : 2;          // чётность сошлась
}

void runExperiment(int infoBits, double Q, int N, const string& label) {
    int correct = 0, detected = 0, undetected = 0;
    for (int t = 0; t < N; t++) {
        int r = runOnePacket(infoBits, Q);
        if (r == 0) correct++;
        else if (r == 1) detected++;
        else undetected++;
    }
    int totalBits = infoBits + 1;
    double R = 1.0 / totalBits;

    // Теоретические оценки
    double pNoErr = pow(1.0 - Q, totalBits);
    double pDetect = (1.0 - pow(1.0 - 2.0 * Q, totalBits)) / 2.0;
    double pUndetect = 1.0 - pNoErr - pDetect;

    cout << "\n================ " << label << " ================\n";
    cout << "Пакет: " << totalBits << " бит (" << infoBits
        << " инф. + 1 чётн.)\n";
    cout << "Избыточность: " << fixed << setprecision(2)
        << R * 100 << " %\n";
    cout << "Испытаний: " << N << ", Q = " << Q << "\n\n";

    cout << "Эксперимент:\n";
    cout << "  Верно передано:        " << correct
        << " (" << 100.0 * correct / N << " %)\n";
    cout << "  Обнаружено:            " << detected
        << " (" << 100.0 * detected / N << " %)\n";
    cout << "  НЕ обнаружено:         " << undetected
        << " (" << 100.0 * undetected / N << " %)\n";
    cout << "  Успешно (верно+обн.):  " << (correct + detected)
        << " (" << 100.0 * (correct + detected) / N << " %)\n";

    cout << "\nТеория:\n";
    cout << "  Верно:        " << pNoErr * 100 << " %\n";
    cout << "  Обнаружено:   " << pDetect * 100 << " %\n";
    cout << "  Не обнаружено:" << pUndetect * 100 << " %\n";
    cout << "  Успешно:      " << (pNoErr + pDetect) * 100 << " %\n";
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand(time(NULL));

    double Q = 0.1;
    int N = 100000;

    cout << "Параметры: Q = " << Q << ", N = " << N << "\n";

    // ---- Задание 3.3: пакеты по 16 бит ----
    runExperiment(16, Q, N, "Задание 3.3. Пакеты 16 бит");

    // ---- Задание 3.4: пакеты по 8 и 4 бита ----
    runExperiment(8, Q, N, "Задание 3.4. Пакеты 8 бит");
    runExperiment(4, Q, N, "Задание 3.4. Пакеты 4 бита");

    return 0;
}