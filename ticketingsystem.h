#ifndef TICKETING_SYSTEM_H
#define TICKETING_SYSTEM_H

#include <QString>
#include <QVector>
#include "passengerhashtable.h"
#include "flightavltree.h"
#include "ticketlist.h"
#include "passenger.h"
#include "flight.h"
#include "ticketrecord.h"
#include "errorhandler.h"
#include "textsearch.h"

class TicketingSystem {
public:
    explicit TicketingSystem(ErrorHandler *errorHandler);
    ~TicketingSystem();

    // Пассажиры
    bool registerPassenger(const Passenger &p);
    bool removePassenger(const QString &passportNumber);
    QVector<Passenger> allPassengers() const;
    void clearPassengers();
    // Поиск пассажира
    Passenger* findPassengerByPassport(const QString &passportNumber, QVector<QPair<QString, QString>> &outFlightTickets) const;
    QVector<QPair<QString, QString>> findPassengersByName(const QString &fullName) const;

    // Рейсы
    bool addFlight(const Flight &f);
    bool removeFlight(const QString &flightNumber);
    QVector<Flight> allFlights() const;
    void clearFlights();
    // Поиск рейса
    Flight* findFlightByNumber(const QString &flightNumber, QVector<QPair<QString, QString>> &outPassengers) const;
    QVector<Flight> findFlightsByArrivalFragment(const QString &fragment) const;

    // Билеты
    bool sellTicket(const QString &passportNumber,
                    const QString &flightNumber,
                    const QString &ticketNumber);
    bool returnTicket(const QString &ticketNumber);
    QVector<TicketRecord> exportAllTickets() const;
    void clearTickets();

    ErrorHandler* GetErrorHandler() const { return errorHandler; }

private:
    PassengerHashTable *passTable;
    FlightAVLTree    *flightTree;
    TicketList       *ticketList;
    ErrorHandler *errorHandler;

};

#endif // TICKETING_SYSTEM_H
