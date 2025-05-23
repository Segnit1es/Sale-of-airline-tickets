#ifndef PASSENGERHASHTABLE_H
#define PASSENGERHASHTABLE_H

#include "passenger.h"
#include <QVector>
#include <QList>

class PassengerHashTable {
private:
    static const int DEFAULT_SIZE = 103;
    QVector<QList<Passenger>> table;
    int HashFunction(const QString& passportNumber) const;

public:
    PassengerHashTable();
    bool Insert(const Passenger& passenger);
    bool Remove(const QString& passportNumber);
    Passenger* Find(const QString& passportNumber);
    void Clear();
    void Traverse(const std::function<void(const Passenger&)>& callback) const;
};

#endif // PASSENGERHASHTABLE_H
