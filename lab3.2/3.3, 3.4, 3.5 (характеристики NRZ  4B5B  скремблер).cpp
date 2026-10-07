#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <windows.h>
#include <clocale>
using namespace std;

string utf8_to_cp1251(const string& utf8) {
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
    wstring w(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &w[0], wlen);
    int clen = WideCharToMultiByte(1251, 0, w.c_str(), -1, NULL, 0, NULL, NULL);
    string r(clen - 1, 0);
    WideCharToMultiByte(1251, 0, w.c_str(), -1, &r[0], clen, NULL, NULL);
    return r;
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

string to4B5B(const string& bin) {
    static const map<string, string> T = {
        { "0000","11110" },{ "0001","01001" },{ "0010","10100" },{ "0011","10101" },
        { "0100","01010" },{ "0101","01011" },{ "0110","01110" },{ "0111","01111" },
        { "1000","10010" },{ "1001","10011" },{ "1010","10110" },{ "1011","10111" },
        { "1100","11010" },{ "1101","11011" },{ "1110","11100" },{ "1111","11101" }
    };
    string b = bin;
    while (b.size() % 4) b += '0';
    string out;
    for (size_t i = 0; i < b.size(); i += 4) {
        auto it = T.find(b.substr(i, 4));
        out += (it != T.end()) ? it->second : "?????";
    }
    return out;
}

string scramble(const string& A) {
    int n = A.size();
    vector<int> a(n), b(n, 0);
    for (int i = 0; i < n; i++) a[i] = A[i] - '0';
    for (int i = 0; i < n; i++) {
        int v = a[i];
        if (i - 3 >= 0) v ^= b[i - 3];
        if (i - 5 >= 0) v ^= b[i - 5];
        b[i] = v;
    }
    string out;
    for (int x : b) out += (x ? '1' : '0');
    return out;
}

void analyze(const string& bin, double C, const string& label) {
    int N = bin.size();
    vector<int> runs(N, 1);
    int i = 0;
    while (i < N) {
        int j = i;
        while (j < N && bin[j] == bin[i]) j++;
        for (int k = i; k < j; k++) runs[k] = j - i;
        i = j;
    }
    int R = *max_element(runs.begin(), runs.end());

    double fHigh = C / 2.0;
    double fLow = C / (2.0 * R);
    double sum = 0;
    for (int r : runs) sum += C / (2.0 * r);
    double fAvg = sum / N;
    double S = fHigh - fLow;
    double F = S * 1.1;

    cout << "\n===== " << label << " =====\n";
    cout << "Сообщение (" << N << " бит):\n" << bin << "\n";
    cout << "Макс. серия R = " << R << "\n";
    cout << fixed << setprecision(3);
    cout << "f_high = " << fHigh / 1e6 << " МГц\n";
    cout << "f_low  = " << fLow / 1e6 << " МГц\n";
    cout << "f_avg  = " << fAvg / 1e6 << " МГц\n";
    cout << "S      = " << S / 1e6 << " МГц\n";
    cout << "F      = " << F / 1e6 << " МГц\n";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "ru_RU.UTF-8");

    // Ваши данные
    string text_cp1251 = utf8_to_cp1251("Надолина К.Р. 23.01.2005");
    string hex;
    for (unsigned char c : text_cp1251) {
        char buf[8]; sprintf_s(buf, "%02X", c);
        hex += buf;
    }

    string bin = hexToBin(hex);
    cout << "HEX: " << hex << "\n";
    cout << "BIN (" << bin.size() << " бит)\n";
    double C = 1e6;

    // 3.3 — исходное (NRZ)
    analyze(bin, C, "Задание 3.3. Исходное (NRZ)");

    // 3.4 — 4B/5B
    string b45 = to4B5B(bin);
    analyze(b45, C * 5.0 / 4.0, "Задание 3.4. 4B/5B");

    // 3.5 — скремблер
    string scr = scramble(b45);
    analyze(scr, C * 5.0 / 4.0, "Задание 3.5. 4B/5B + скремблер");

    return 0;
}