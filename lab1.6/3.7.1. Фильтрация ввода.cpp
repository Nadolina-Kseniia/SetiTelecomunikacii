#include <iostream>
#include <string>
#include <clocale>
using namespace std;

bool isBinary(const string& s) {
    if (s.empty()) return false;
    for (char c : s) if (c != '0' && c != '1') return false;
    return true;
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    string s;
    do {
        cout << "Введите последовательность из 0 и 1: ";
        cin >> s;
        if (!isBinary(s))
            cout << "Ошибка: допустимы только 0 и 1!\n";
    } while (!isBinary(s));

    cout << "Принято: " << s << "\n";
    return 0;
}