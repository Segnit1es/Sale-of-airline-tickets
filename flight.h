#ifndef FLIGHT_H
#define FLIGHT_H

#include <QString>
#include <memory>
#include "errorhandler.h"

class Flight {
public:
    Flight(const QString &flightNumber,
           const QString &airlineName,
           const QString &departureAirport,
           const QString &arrivalAirport,
           const QString &departureDate,
           const QString &departureTime,
           int totalSeats,
           int freeSeats,
           ErrorHandler *errorHandler);

    // Геттеры
    QString GetFlightNumber() const;
    QString GetAirlineName() const;
    QString GetDepartureAirport() const;
    QString GetArrivalAirport() const;
    QString GetDepartureDate() const;
    QString GetDepartureTime() const;
    int GetTotalSeats() const;
    int GetFreeSeats() const;

    // Работа с местами
    bool ReserveSeat();
    bool ReleaseSeat();

    bool IsValid() const { return valid; }

private:
    QString flightNumber;
    QString airlineName;
    QString departureAirport;
    QString arrivalAirport;
    QString departureDate;
    QString departureTime;
    int totalSeats;
    int freeSeats;

    ErrorHandler *errorHandler;

    bool ValidateData();
    bool valid = true;
};

#endif // FLIGHT_H
