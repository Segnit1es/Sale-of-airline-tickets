#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <QString>
#include <QRegularExpression>

class Validator {
public:
    static bool IsValidPassportNumber(const QString &passportNumber);
    static bool IsValidFullName(const QString &fullName);
    static bool IsValidDate(const QString &date);
    static bool IsValidIssuePlace(const QString &place);
    static bool IsValidTicketNumber(const QString &ticketNumber);

    static bool IsValidFlightNumber(const QString &flightNumber);
    static bool IsValidTime(const QString &time);
    static bool IsNonEmptyString(const QString &s);
};

#endif // VALIDATOR_H
