#include "validator.h"

bool Validator::IsValidPassportNumber(const QString &passportNumber) {
    static const QRegularExpression regex("^\\d{4}-\\d{6}$");
    return regex.match(passportNumber).hasMatch();
}

bool Validator::IsValidFullName(const QString &fullName) {
    static const QRegularExpression regex("^[А-Яа-яA-Za-z\\s\\-]+$");
    return regex.match(fullName).hasMatch() && fullName.trimmed().split(' ').size() >= 2;
}

bool Validator::IsValidDate(const QString &date) {
    static const QRegularExpression regex("^\\d{2}\\.\\d{2}\\.\\d{4}$");
    return regex.match(date).hasMatch();
}

bool Validator::IsValidIssuePlace(const QString &place) {
    return !place.trimmed().isEmpty();
}
bool Validator::IsValidFlightNumber(const QString &flightNumber) {
    // Формат "AAA-NNN", где AAA — три латинские буквы, NNN — 1–4 цифры
    static const QRegularExpression regex("^[A-Z]{3}-\\d{1,4}$");
    return regex.match(flightNumber).hasMatch();
}

bool Validator::IsValidTime(const QString &time) {
    // Формат "HH:MM", 00–23:00–59
    static const QRegularExpression regex("^([01]\\d|2[0-3]):[0-5]\\d$");
    return regex.match(time).hasMatch();
}

bool Validator::IsNonEmptyString(const QString &s) {
    return !s.trimmed().isEmpty();
}
bool Validator::IsValidTicketNumber(const QString &ticketNumber) {
    static const QRegularExpression regex("^\\d{9}$");
    return regex.match(ticketNumber).hasMatch();
}
