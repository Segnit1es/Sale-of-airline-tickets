#include "ticketingsystem.h"
#include "validator.h"
#include <QMessageBox>

TicketingSystem::TicketingSystem(ErrorHandler *err)
    : passTable(new PassengerHashTable())
    , flightTree(new FlightAVLTree())
    , ticketList(new TicketList(err))
    , errorHandler(err)
{}

TicketingSystem::~TicketingSystem() {
    delete passTable;
    delete flightTree;
    delete ticketList;
}


bool TicketingSystem::registerPassenger(const Passenger &p) {
    if (!passTable->Insert(p)) {
        errorHandler->RaiseError("Пассажир с таким паспортом уже существует", "registerPassenger");
        return false;
    }
    return true;
}

bool TicketingSystem::removePassenger(const QString &passportNumber) {
    QVector<QPair<QString, QString>> dummyFlights;
    Passenger *p = findPassengerByPassport(passportNumber, dummyFlights);
    if (!p) {
        return false;
    }
    if (!dummyFlights.isEmpty()) {
        errorHandler->RaiseError("Нельзя удалить: у пассажира есть купленные билеты", "removePassenger");
        return false;
    }
    if (!passTable->Remove(passportNumber)) {
        errorHandler->RaiseError("Пассажир не найден", "removePassenger");
        return false;
    }
    return true;
}

QVector<Passenger> TicketingSystem::allPassengers() const {
    QVector<Passenger> result;
    passTable->Traverse([&](const Passenger &p){
        result.append(p);
    });
    return result;
}

void TicketingSystem::clearPassengers() {
    passTable->Clear();
}

// Поиск пассажира по паспорту
Passenger* TicketingSystem::findPassengerByPassport(
    const QString &passportNumber,
    QVector<QPair<QString, QString>> &outFlightTickets
    ) const
{
    Passenger *p = passTable->Find(passportNumber);
    if (!p) {
        errorHandler->RaiseError("Пассажир не найден", "findPassengerByPassport");
        return nullptr;
    }
    outFlightTickets.clear();
    ticketList->Traverse([&](const TicketRecord &rec){
        if (rec.GetPassportNumber() == passportNumber) {
            outFlightTickets.append(qMakePair(
                rec.GetFlightNumber(),
                rec.GetTicketNumber()
                ));
        }
    });
    return p;
}

// Поиск пассажиров по ФИО
QVector<QPair<QString, QString>> TicketingSystem::findPassengersByName(const QString &fullName) const {
    QVector<QPair<QString, QString>> result;
    passTable->Traverse([&](const Passenger &p){
        if (p.GetFullName().contains(fullName, Qt::CaseInsensitive))
            result.append(qMakePair(p.GetPassportNumber(), p.GetFullName()));
    });
    if (result.isEmpty()) {
        errorHandler->RaiseError("Пассажиров с таким ФИО не найдено", "findPassengersByName");
    }
    return result;
}


bool TicketingSystem::addFlight(const Flight &f) {
    auto ptr = std::make_shared<Flight>(f);
    if (!flightTree->Insert(ptr)) {
        errorHandler->RaiseError("Рейс с таким номером уже существует", "addFlight");
        return false;
    }
    return true;
}

bool TicketingSystem::removeFlight(const QString &flightNumber) {
    int soldCount = 0;
    ticketList->Traverse([&](const TicketRecord &rec){
        if (rec.GetFlightNumber() == flightNumber)
            ++soldCount;
    });
    if (soldCount > 0) {
        errorHandler->RaiseError("Нельзя удалить: на рейс куплены билеты", "removeFlight");
        return false;
    }
    if (!flightTree->Remove(flightNumber)) {
        errorHandler->RaiseError("Рейс не найден", "removeFlight");
        return false;
    }
    return true;
}

QVector<Flight> TicketingSystem::allFlights() const {
    QVector<Flight> result;
    flightTree->TraverseReverse([&](const std::shared_ptr<Flight> &f){
        result.append(*f);
    });
    return result;
}

void TicketingSystem::clearFlights() {
    flightTree->Clear();
}

// Поиск рейса по номеру
Flight* TicketingSystem::findFlightByNumber(
    const QString &flightNumber,
    QVector<QPair<QString, QString>> &outPassengers) const
{
    auto ptr = flightTree->Find(flightNumber);
    if (!ptr) {
        errorHandler->RaiseError("Рейс не найден", "findFlightByNumber");
        return nullptr;
    }
    outPassengers.clear();
    ticketList->Traverse([&](const TicketRecord &rec){
        if (rec.GetFlightNumber() == flightNumber) {
            Passenger *p = passTable->Find(rec.GetPassportNumber());
            if (p)
                outPassengers.append(qMakePair(p->GetPassportNumber(), p->GetFullName()));
        }
    });
    return ptr.get();
}

// Поиск рейсов по фрагменту аэропорта прибытия
QVector<Flight> TicketingSystem::findFlightsByArrivalFragment(const QString &fragment) const {
    QVector<Flight> result;
    flightTree->TraverseReverse([&](const std::shared_ptr<Flight> &f){
        if (TextSearch::BoyerMooreSearch(f->GetArrivalAirport(), fragment))
            result.append(*f);
    });
    if (result.isEmpty()) {
        errorHandler->RaiseError("Рейсов не найдено по заданному фрагменту", "findFlightsByArrivalFragment");
    }
    return result;
}


bool TicketingSystem::sellTicket(const QString &passportNumber,
                                 const QString &flightNumber,
                                 const QString &ticketNumber)
{
    // 1) Проверяем, что на таком рейсе есть свободные места и он существует
    auto flightPtr = flightTree->Find(flightNumber);
    if (!flightPtr) {
        errorHandler->RaiseError(
            QString("Рейс %1 не найден").arg(flightNumber),
            "sellTicket"
            );
        return false;
    }
    if (flightPtr->GetFreeSeats() <= 0) {
        errorHandler->RaiseError(
            QString("На рейсе %1 нет свободных мест").arg(flightNumber),
            "sellTicket"
            );
        return false;
    }

    // 2) Проверка уникальности номера билета
    bool exists = false;
    ticketList->Traverse([&](const TicketRecord &r) {
        if (r.GetTicketNumber() == ticketNumber)
            exists = true;
    });
    if (exists) {
        errorHandler->RaiseError(
            QString("Билет с номером %1 уже продан").arg(ticketNumber),
            "sellTicket"
            );
        return false;
    }

    // 3) Создаём и валидируем запись билета (внутри TicketRecord тоже может проверяться корректность)
    TicketRecord rec(
        passportNumber,
        flightNumber,
        ticketNumber,
        passTable,
        flightTree,
        errorHandler
        );
    if (!rec.IsValid()) {
        // Внутри конструктора TicketRecord вызовет RaiseError
        return false;
    }

    // 4) Резервируем место и добавляем в список
    if (!flightPtr->ReserveSeat()) {
        // на всякий случай — но обычно уже проверили выше
        errorHandler->RaiseError(
            QString("Не удалось зарезервировать место на рейсе %1").arg(flightNumber),
            "sellTicket"
            );
        return false;
    }
    ticketList->Append(rec);
    return true;
}

bool TicketingSystem::returnTicket(const QString &ticketNumber) {
    bool removed = ticketList->RemoveByTicketNumber(ticketNumber, [&](const TicketRecord &rec){
        auto flightPtr = flightTree->Find(rec.GetFlightNumber());
        if (flightPtr) flightPtr->ReleaseSeat();
    });
    if (!removed) {
        errorHandler->RaiseError("Номер билета не найден", "returnTicket");
        return false;
    }
    return true;
}

QVector<TicketRecord> TicketingSystem::exportAllTickets() const {
    QVector<TicketRecord> all;
    ticketList->Traverse([&](const TicketRecord &rec){
        all.append(rec);
    });
    return all;
}
void TicketingSystem::clearTickets() {

    ticketList->Clear();

}
