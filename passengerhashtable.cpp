#include "passengerhashtable.h"

PassengerHashTable::PassengerHashTable() {
    table.resize(DEFAULT_SIZE);
}

int PassengerHashTable::HashFunction(const QString& passportNumber) const {
    uint hash = 0;
    for (QChar ch : passportNumber) {
        hash = hash * 31 + ch.unicode();
    }
    return hash % DEFAULT_SIZE;
}

bool PassengerHashTable::Insert(const Passenger& passenger) {
    int index = HashFunction(passenger.GetPassportNumber());
    for (const Passenger& p : table[index]) {
        if (p.GetPassportNumber() == passenger.GetPassportNumber())
            return false; // Уже существует
    }
    table[index].append(passenger);
    return true;
}

bool PassengerHashTable::Remove(const QString& passportNumber) {
    int index = HashFunction(passportNumber);
    auto& list = table[index];
    for (auto it = list.begin(); it != list.end(); ++it) {
        if (it->GetPassportNumber() == passportNumber) {
            list.erase(it);
            return true;
        }
    }
    return false;
}

Passenger* PassengerHashTable::Find(const QString& passportNumber) {
    int index = HashFunction(passportNumber);
    for (Passenger& p : table[index]) {
        if (p.GetPassportNumber() == passportNumber)
            return &p;
    }
    return nullptr;
}

void PassengerHashTable::Clear() {
    for (auto& list : table)
        list.clear();
}
void PassengerHashTable::Traverse(const std::function<void(const Passenger&)>& callback) const {
    for (const auto& bucket : table) {
        for (const Passenger& p : bucket) {
            callback(p);
        }
    }
}
