#include <iostream>
#include <string>
#include <windows.h>
#include <clocale>
using namespace std;

string utf8_to_cp1251(const string& utf8) {
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
    wstring wstr(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);
    int clen = WideCharToMultiByte(1251, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    string cp1251(clen - 1, 0);
    WideCharToMultiByte(1251, 0, wstr.c_str(), -1, &cp1251[0], clen, NULL, NULL);
    return cp1251;
}

string cp1251_to_utf8(const string& cp1251) {
    int wlen = MultiByteToWideChar(1251, 0, cp1251.c_str(), -1, NULL, 0);
    wstring wstr(wlen, 0);
    MultiByteToWideChar(1251, 0, cp1251.c_str(), -1, &wstr[0], wlen);
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    string utf8(len - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8[0], len, NULL, NULL);
    return utf8;
}

string hexToBin(const string& hex) {
    string bin;
    for (char c : hex) {
        int v;
        if (c >= '0' && c <= '9')      v = c - '0';
        else if (c >= 'A' && c <= 'F') v = c - 'A' + 10;
        else if (c >= 'a' && c <= 'f') v = c - 'a' + 10;
        else continue;
        for (int i = 3; i >= 0; --i)
            bin += ((v >> i) & 1) ? '1' : '0';
    }
    return bin;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "ru_RU.UTF-8");

    string text_cp1251 = utf8_to_cp1251("Надолина К.Р. 23.01.2005");
    cout << "Исходная строка: " << cp1251_to_utf8(text_cp1251) << "\n\n";

    string hex;
    cout << "Символ | dec | hex\n";
    cout << "-------|-----|----\n";
    for (unsigned char c : text_cp1251) {
        char buf[8]; sprintf_s(buf, "%02X", c);
        string one(1, (char)c);
        cout << "   " << cp1251_to_utf8(one) << "    |  "
            << (int)c << "  | " << buf << "\n";
        hex += buf;
    }

    cout << "\nHEX-строка:\n" << hex << "\n";

    string bin = hexToBin(hex);
    cout << "\nДвоичный код (" << bin.size() << " бит):\n" << bin << "\n";

    while (bin.size() % 16) bin += '0';
    int n = bin.size() / 16;
    cout << "\nПакеты по 16 бит (всего " << n << "):\n";
    for (int i = 0; i < n; i++)
        cout << "  Пакет " << (i + 1) << ": " << bin.substr(i * 16, 16) << "\n";
    return 0;
}