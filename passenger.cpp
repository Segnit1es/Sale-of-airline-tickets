#include "passenger.h"
#include "validator.h"
#include <stdexcept>

Passenger::Passenger(const QString &passportNumber,
                     const QString &issuePlaceAndDate,
                     const QString &fullName,
                     const QString &birthDate,
                     ErrorHandler *errorHandler)
    : passportNumber(passportNumber)
    , issuePlaceAndDate(issuePlaceAndDate)
    , fullName(fullName)
    , birthDate(birthDate)
    , errorHandler(errorHandler)
{
    if (!ValidateData()) {
        valid = false;
    }
}

bool Passenger::ValidateData()
{
    static const QRegularExpression rxPassport(R"(^\d{4}-\d{6}$)");
    if (!rxPassport.match(passportNumber).hasMatch()) {
        errorHandler->RaiseError("Неверный формат номера паспорта (NNNN-NNNNNN)", "Passenger");
        return false;
    }
    if (issuePlaceAndDate.trimmed().isEmpty()) {
        errorHandler->RaiseError("Место и дата выдачи паспорта не могут быть пустыми", "Passenger");
        return false;
    }
    if (!Validator::IsNonEmptyString(fullName)) {
        errorHandler->RaiseError("ФИО не может быть пустым", "Passenger");
        return false;
    }
    if (!Validator::IsValidDate(birthDate)) {
        errorHandler->RaiseError("Неверный формат даты рождения (ДД.MM.ГГГГ)", "Passenger");
        return false;
    }
    return true;
}
