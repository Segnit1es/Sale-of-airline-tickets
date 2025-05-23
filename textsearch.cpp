#include "textsearch.h"

// bad-character таблица
static std::vector<int> makeBadCharTable(const QString &pat) {
    const int ALPHABET = 256;
    int m = pat.length();
    std::vector<int> table(ALPHABET, m);
    for (int i = 0; i < m - 1; ++i) {
        unsigned char c = static_cast<unsigned char>(pat[i].toLatin1());
        table[c] = m - 1 - i;
    }
    return table;
}

// good-suffix таблица
static std::vector<int> makeGoodSuffixTable(const QString &pat) {
    int m = pat.length();
    std::vector<int> suff(m);
    suff[m - 1] = m;
    int g = m - 1, f = 0;
    for (int i = m - 2; i >= 0; --i) {
        if (i > g && suff[i + m - 1 - f] < i - g)
            suff[i] = suff[i + m - 1 - f];
        else {
            if (i < g) g = i;
            f = i;
            while (g >= 0 && pat[g] == pat[g + m - 1 - f]) --g;
            suff[i] = f - g;
        }
    }
    std::vector<int> table(m, m);
    int j = 0;
    for (int i = m - 1; i >= 0; --i) {
        if (suff[i] == i + 1) {
            while (j < m - 1 - i) {
                table[j] = m - 1 - i;
                ++j;
            }
        }
    }
    for (int i = 0; i <= m - 2; ++i) {
        int idx = m - 1 - suff[i];
        table[idx] = m - 1 - i;
    }
    return table;
}

bool TextSearch::BoyerMooreSearch(const QString &text, const QString &pattern) {
    int n = text.length();
    int m = pattern.length();
    if (m == 0) return true;
    if (n < m) return false;

    std::vector<int> badChar = makeBadCharTable(pattern);
    std::vector<int> goodSuf = makeGoodSuffixTable(pattern);

    int s = 0;
    while (s <= n - m) {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[s + j]) {
            --j;
        }
        if (j < 0) {
            return true;
        } else {
            unsigned char c = static_cast<unsigned char>(text[s + j].toLatin1());
            int bcShift = badChar[c] - (m - 1 - j);
            if (bcShift < 1) bcShift = 1;
            int gsShift = goodSuf[j];
            s += (bcShift > gsShift ? bcShift : gsShift);
        }
    }
    return false;
}
