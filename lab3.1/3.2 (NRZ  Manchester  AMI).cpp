#include <iostream>
#include <string>
#include <clocale>
using namespace std;

/*1100110111*/

void showNRZ(const string& bits) {
    cout << "\n--- NRZ ---\n";
    cout << "Бит:    ";
    for (char c : bits) cout << " " << c << "  ";
    cout << "\nСигнал: ";
    for (char c : bits) cout << (c == '1' ? "--- " : "___ ");
    cout << "\n";
}

void showManchester(const string& bits) {
    cout << "\n--- Манчестер (1 = B->H, 0 = H->B) ---\n";
    cout << "Бит:    ";
    for (char c : bits) cout << " " << c << "  ";
    cout << "\nСигнал: ";
    for (char c : bits) cout << (c == '1' ? "-|_ " : "_|- ");
    cout << "\n";
}

void showAMI(const string& bits) {
    cout << "\n--- AMI ---\n";
    cout << "Бит:    ";
    for (char c : bits) cout << " " << c << "  ";
    cout << "\nСигнал: ";
    int sign = 1;
    for (char c : bits) {
        if (c == '1') { cout << (sign > 0 ? " +  " : " -  "); sign = -sign; }
        else cout << " 0  ";
    }
    cout << "\n";
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    string bits;
    cout << "Введите первые 10 бит: ";
    cin >> bits;
    if (bits.size() != 10) { cout << "Нужно 10 бит\n"; return 1; }
    for (char c : bits) if (c != '0' && c != '1') { cout << "Только 0 и 1\n"; return 1; }

    showNRZ(bits);
    showManchester(bits);
    showAMI(bits);
    return 0;
}