#ifndef PASSENGER_H
#define PASSENGER_H

#include <QString>
#include <QRegularExpression>
#include <memory>
#include "errorhandler.h"

class Passenger {
public:
    Passenger(const QString &passportNumber,
              const QString &issuePlaceAndDate,
              const QString &fullName,
              const QString &birthDate,
              ErrorHandler *errorHandler);

    QString GetPassportNumber()       const { return passportNumber; }
    QString GetIssuePlaceAndDate()    const { return issuePlaceAndDate; }
    QString GetFullName()             const { return fullName; }
    QString GetBirthDate()            const { return birthDate; }
     bool IsValid() const { return valid; }

private:
    QString passportNumber;
    QString issuePlaceAndDate;
    QString fullName;
    QString birthDate;

    ErrorHandler *errorHandler;

    bool ValidateData();
    bool valid = true;
};

#endif // PASSENGER_H
