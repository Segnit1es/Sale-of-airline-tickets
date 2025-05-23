#include "database.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Database::Database(TicketingSystem *ts)
    : ts_(ts)
{}

Database::~Database() = default;

bool Database::createNew(const QString &filePath)
{
    QFile f(filePath);
    if (f.exists())
        f.remove();
    setCurrentFilePath(filePath);
    return f.open(QIODevice::WriteOnly);  // пустой файл создан

}

bool Database::save(const QString &filePath)
{
    // 1) Собираем JSON-структуру
    QJsonObject root;

    // Пассажиры
    QJsonArray arrPassengers;
    for (auto &p : ts_->allPassengers()) {
        QJsonObject o;
        o["passportNumber"]   = p.GetPassportNumber();
        o["issuePlaceAndDate"]= p.GetIssuePlaceAndDate();
        o["fullName"]         = p.GetFullName();
        o["birthDate"]        = p.GetBirthDate();
        arrPassengers.append(o);
    }
    root["passengers"] = arrPassengers;

    // Рейсы
    QJsonArray arrFlights;
    for (auto &f : ts_->allFlights()) {
        QJsonObject o;
        o["flightNumber"]   = f.GetFlightNumber();
        o["airlineName"]    = f.GetAirlineName();
        o["departureAirport"]= f.GetDepartureAirport();
        o["arrivalAirport"] = f.GetArrivalAirport();
        o["departureDate"]  = f.GetDepartureDate();
        o["departureTime"]  = f.GetDepartureTime();
        o["totalSeats"]     = f.GetTotalSeats();
        o["freeSeats"]      = f.GetFreeSeats();
        arrFlights.append(o);
    }
    root["flights"] = arrFlights;

    // Билеты
    QJsonArray arrTickets;
    for (auto &r : ts_->exportAllTickets()) {
        QJsonObject o;
        o["passportNumber"] = r.GetPassportNumber();
        o["flightNumber"]   = r.GetFlightNumber();
        o["ticketNumber"]   = r.GetTicketNumber();
        arrTickets.append(o);
    }
    root["tickets"] = arrTickets;

    // 2) Сериализуем в текст JSON
    QJsonDocument doc(root);
    QByteArray data = doc.toJson(QJsonDocument::Indented);

    // 3) Записываем в файл
    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    f.write(data);
    f.close();

    setCurrentFilePath(filePath);
    return true;
}

bool Database::load(const QString &filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
        return false;
    QByteArray data = f.readAll();
    f.close();

    QJsonParseError errParse;
    QJsonDocument doc = QJsonDocument::fromJson(data, &errParse);
    if (errParse.error != QJsonParseError::NoError || !doc.isObject())
        return false;

    QJsonObject root = doc.object();

    // 1) Очищаем текущее состояние
    ts_->clearPassengers();
    ts_->clearFlights();
    ts_->clearTickets();
    // (список билетов внутри TicketingSystem тоже очищается в clearPassengers/clearFlights)

    // 2) Загружаем пассажиров
    if (root.contains("passengers") && root["passengers"].isArray()) {
        for (auto v : root["passengers"].toArray()) {
            auto o = v.toObject();
            Passenger p(
                o["passportNumber"].toString(),
                o["issuePlaceAndDate"].toString(),
                o["fullName"].toString(),
                o["birthDate"].toString(),
                ts_->GetErrorHandler()
                );
            ts_->registerPassenger(p);
        }
    }

    // 3) Загружаем рейсы
    if (root.contains("flights") && root["flights"].isArray()) {
        for (auto v : root["flights"].toArray()) {
            auto o = v.toObject();
            Flight f(
                o["flightNumber"].toString(),
                o["airlineName"].toString(),
                o["departureAirport"].toString(),
                o["arrivalAirport"].toString(),
                o["departureDate"].toString(),
                o["departureTime"].toString(),
                o["totalSeats"].toInt(),
                o["freeSeats"].toInt(),
                ts_->GetErrorHandler()
                );
            ts_->addFlight(f);
        }
    }

    // 4) Загружаем билеты
    if (root.contains("tickets") && root["tickets"].isArray()) {
        for (auto v : root["tickets"].toArray()) {
            auto o = v.toObject();
            ts_->sellTicket(
                o["passportNumber"].toString(),
                o["flightNumber"].toString(),
                o["ticketNumber"].toString()
                );
        }
    }

    setCurrentFilePath(filePath);
    return true;
}

bool Database::removeFile(const QString &filePath)
{
    currentFilePath.clear();
    return QFile::remove(filePath);
}

bool Database::copy(const QString &srcPath, const QString &dstPath)
{
    // создаём директорию-приёмник, если нужно
    QFileInfo dstInfo(dstPath);
    QDir().mkpath(dstInfo.absolutePath());
    setCurrentFilePath(dstPath);
    return QFile::copy(srcPath, dstPath);
}

void Database::setCurrentFilePath(const QString &path)
{
    currentFilePath = path;
}

QString Database::getCurrentFilePath() const
{
    return currentFilePath;
}
QString Database::currentFileName() const
{
    if (currentFilePath.isEmpty())
        return QString();
    return QFileInfo(currentFilePath).fileName();
}

