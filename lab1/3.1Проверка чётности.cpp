#include <iostream>
#include <cstdlib>
#include <ctime>
#include <clocale>
using namespace std;

int main() {
    /*Задание: вводить последовательность одним числом (например, 10010), не допуская символы кроме 0 и 1.*/

    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand(time(NULL));
    int data[5];
    cout << "Введите 4 бита (0/1): ";
    for (int i = 0; i < 4; i++) cin >> data[i];

    int parity = 0;
    for (int i = 0; i < 4; i++) parity ^= data[i];
    data[4] = parity;

    cout << "Передано: ";
    for (int i = 0; i < 4; i++) cout << data[i];
    cout << endl;

    double Q;
    cout << "Введите вероятность ошибки Q: ";
    cin >> Q;

    for (int i = 0; i < 5; i++) {
        if ((double)rand() / RAND_MAX < Q) data[i] ^= 1;
    }

    cout << "Принято: ";
    for (int i = 0; i < 4; i++) cout << data[i];
    cout << endl;

    int sum = 0;
    for (int i = 0; i < 5; i++) sum ^= data[i];

    if (sum == 0)
        cout << "Ошибок нет (или чётное число ошибок)" << endl;
    else
        cout << "Обнаружена ошибка" << endl;

    return 0;
}