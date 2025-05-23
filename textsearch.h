#ifndef TEXTSEARCH_H
#define TEXTSEARCH_H

#include <QString>
#include <vector>

class TextSearch {
public:
    // Возвращает true, если pattern найден в text
    static bool BoyerMooreSearch(const QString &text, const QString &pattern);
};

#endif // TEXTSEARCH_H
