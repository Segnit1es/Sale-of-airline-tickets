#include "ticketrecord.h"
#include "validator.h"

TicketRecord::TicketRecord(const QString &passportNumber,
                           const QString &flightNumber,
                           const QString &ticketNumber,
                           PassengerHashTable *passengers,
                           FlightAVLTree *flights,
                           ErrorHandler *errorHandler)
    : passportNumber(passportNumber),
    flightNumber(flightNumber),
    ticketNumber(ticketNumber)
{
    if (!ValidateData(passengers, flights, errorHandler)) {
        isValid=false;
    }
}

bool TicketRecord::ValidateData(PassengerHashTable *passengers,
                                FlightAVLTree *flights,
                                ErrorHandler *errorHandler)
{
    // 1) Проверяем, что пассажир зарегистрирован
    if (!passengers->Find(passportNumber)) {
        errorHandler->RaiseError(
            QString("Пассажир с паспортом %1 не найден").arg(passportNumber),
            "TicketRecord");
        return false;
    }

    // 2) Проверяем, что рейс существует
    if (!flights->Find(flightNumber)) {
        errorHandler->RaiseError(
            QString("Рейс с номером %1 не найден").arg(flightNumber),
            "TicketRecord");
        return false;
    }

    // 3) Проверяем формат номера билета (осталась проверка по 9 цифрам)
    if (!Validator::IsValidTicketNumber(ticketNumber)) {
        errorHandler->RaiseError(
            QString("Неверный формат номера авиабилета '%1' (должно быть 9 цифр)")
                .arg(ticketNumber),
            "TicketRecord");
        return false;
    }

    return true;
}

QString TicketRecord::GetPassportNumber() const {
    return passportNumber;
}

QString TicketRecord::GetFlightNumber() const {
    return flightNumber;
}

QString TicketRecord::GetTicketNumber() const {
    return ticketNumber;
}
