#ifndef TICKETRECORD_H
#define TICKETRECORD_H

#include <QString>
#include "errorhandler.h"
#include "passengerhashtable.h"
#include "flightavltree.h"

class TicketRecord {
public:
    TicketRecord(const QString &passportNumber,
                 const QString &flightNumber,
                 const QString &ticketNumber,
                 PassengerHashTable *passengers,
                 FlightAVLTree *flights,
                 ErrorHandler *errorHandler);

    QString GetPassportNumber() const;
    QString GetFlightNumber() const;
    QString GetTicketNumber() const;
    bool IsValid() const { return isValid; }

private:
    QString passportNumber;
    QString flightNumber;
    QString ticketNumber;
    bool isValid;

    bool ValidateData(PassengerHashTable *passengers,
                      FlightAVLTree *flights,
                      ErrorHandler *errorHandler);
};

#endif // TICKETRECORD_H
