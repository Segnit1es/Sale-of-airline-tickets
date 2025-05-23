#include "flight.h"
#include "validator.h"

Flight::Flight(const QString &flightNumber,
               const QString &airlineName,
               const QString &departureAirport,
               const QString &arrivalAirport,
               const QString &departureDate,
               const QString &departureTime,
               int totalSeats,
               int freeSeats,
               ErrorHandler *errorHandler)
    : flightNumber(flightNumber),
    airlineName(airlineName),
    departureAirport(departureAirport),
    arrivalAirport(arrivalAirport),
    departureDate(departureDate),
    departureTime(departureTime),
    totalSeats(totalSeats),
    freeSeats(freeSeats),
    errorHandler(errorHandler)
{
    if (!ValidateData()) {
        valid = false;
    }
}

bool Flight::ValidateData()
{
    if (!Validator::IsValidFlightNumber(flightNumber)) {
        errorHandler->RaiseError("Неверный формат номера рейса (ожидается AAA-NNN)", "Flight");
        return false;
    }
    if (!Validator::IsNonEmptyString(airlineName)) {
        errorHandler->RaiseError("Название авиакомпании не может быть пустым", "Flight");
        return false;
    }
    if (!Validator::IsNonEmptyString(departureAirport)) {
        errorHandler->RaiseError("Аэропорт отправления не может быть пустым", "Flight");
        return false;
    }
    if (!Validator::IsNonEmptyString(arrivalAirport)) {
        errorHandler->RaiseError("Аэропорт прибытия не может быть пустым", "Flight");
        return false;
    }
    if (!Validator::IsValidDate(departureDate)) {
        errorHandler->RaiseError("Неверный формат даты отправления (ДД.MM.ГГГГ)", "Flight");
        return false;
    }
    if (!Validator::IsValidTime(departureTime)) {
        errorHandler->RaiseError("Неверный формат времени отправления (ЧЧ:ММ)", "Flight");
        return false;
    }
    if (totalSeats <= 0) {
        errorHandler->RaiseError("Общее количество мест должно быть положительным", "Flight");
        return false;
    }
    if (freeSeats < 0 || freeSeats > totalSeats) {
        errorHandler->RaiseError("Неверное число свободных мест", "Flight");
        return false;
    }
    return true;
}

// Геттеры
QString Flight::GetFlightNumber() const       { return flightNumber; }
QString Flight::GetAirlineName() const        { return airlineName; }
QString Flight::GetDepartureAirport() const   { return departureAirport; }
QString Flight::GetArrivalAirport() const     { return arrivalAirport; }
QString Flight::GetDepartureDate() const      { return departureDate; }
QString Flight::GetDepartureTime() const      { return departureTime; }
int     Flight::GetTotalSeats() const         { return totalSeats; }
int     Flight::GetFreeSeats() const          { return freeSeats; }

// Управление местами
bool Flight::ReserveSeat() {
    if (freeSeats > 0) {
        --freeSeats;
        return true;
    }
    return false;
}

bool Flight::ReleaseSeat() {
    if (freeSeats < totalSeats) {
        ++freeSeats;
        return true;
    }
    return false;
}
