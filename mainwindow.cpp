#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "passengerdialog.h"
#include "flightdialog.h"
#include "ticketdialog.h"
#include "ticketingsystem.h"

#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    err(new ErrorHandler(this))
{
    ui->setupUi(this);
    ticketingSystem = std::make_unique<TicketingSystem>(err);
    db = std::make_unique<Database>(ticketingSystem.get());
    updateUiEnabled();
    ui->passengerTable      ->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->flightTable         ->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->availableFlightTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QFile file(":/styles/darkstyle.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString style = QString::fromUtf8(file.readAll());
        qApp->setStyleSheet(style);
        file.close();
    } else {
        qWarning() << "Не удалось загрузить тему:" << file.errorString();
    }
}

MainWindow::~MainWindow()
{
    if(db->isCurrentFilePathSet()){
        db->save(db->getCurrentFilePath());
    }
    delete ui;
}
void MainWindow::updateUiEnabled()
{

    const bool ok = db->isCurrentFilePathSet();
    if(ok){
        ui->passengersBox->setTitle("");
        ui->flightBox    ->setTitle("");
        ui->ticketBox    ->setTitle("");
    }else{
        ui->passengersBox->setTitle("Функции заблокированы. Создайте или выберите базу данных");
        ui->flightBox    ->setTitle("Функции заблокированы. Создайте или выберите базу данных");
        ui->ticketBox    ->setTitle("Функции заблокированы. Создайте или выберите базу данных");
    }
    ui->passengersBox->setEnabled(ok);
    ui->flightBox    ->setEnabled(ok);
    ui->ticketBox    ->setEnabled(ok);
    ui->saveDB       ->setEnabled(ok);
}
void MainWindow::updateAvailableFlights()
{
    // Получаем все рейсы
    auto flights = ticketingSystem->allFlights();

    auto *model = new QStandardItemModel(flights.size(), 6, this);
    model->setHeaderData(0, Qt::Horizontal, tr("Номер"));
    model->setHeaderData(1, Qt::Horizontal, tr("Авиакомпания"));
    model->setHeaderData(2, Qt::Horizontal, tr("Откуда"));
    model->setHeaderData(3, Qt::Horizontal, tr("Куда"));
    model->setHeaderData(4, Qt::Horizontal, tr("Дата/время"));
    model->setHeaderData(5, Qt::Horizontal, tr("Свободных мест"));

    // Заполняем модель
    for (int i = 0; i < flights.size(); ++i) {
        const auto &f = flights[i];
        model->setItem(i, 0, new QStandardItem(f.GetFlightNumber()));
        model->setItem(i, 1, new QStandardItem(f.GetAirlineName()));
        model->setItem(i, 2, new QStandardItem(f.GetDepartureAirport()));
        model->setItem(i, 3, new QStandardItem(f.GetArrivalAirport()));
        model->setItem(i, 4, new QStandardItem(f.GetDepartureDate() + " " + f.GetDepartureTime()));
        model->setItem(i, 5, new QStandardItem(QString::number(f.GetFreeSeats())));
    }

    // Устанавливаем модель в таблицу и подгоняем ширины колонок
    ui->availableFlightTable->setModel(model);
    ui->availableFlightTable->resizeColumnsToContents();
}

// Пассажиры

void MainWindow::on_addPassenger_clicked()
{
    PassengersDialog dlg(PassengersDialog::Register, err, this);
    connect(&dlg, &PassengersDialog::registered, this,
            [&](const QString &passport,
                const QString &issuePlace,
                const QString &name,
                const QString &birthDate)
            {
                Passenger p(passport, issuePlace, name, birthDate, err);
                // если данные невалидны — просто не регистрируем и выходим
                if (!p.IsValid()) {
                    return;
                }
                // пытаемся добавить в систему
                if (!ticketingSystem->registerPassenger(p)) {
                    return;
                }
            });
    dlg.exec();
}

void MainWindow::on_removePassenger_clicked()
{
    PassengersDialog dlg(PassengersDialog::Remove, err, this);
    connect(&dlg, &PassengersDialog::removed, this, [&](const QString &passport){
        if (!ticketingSystem->removePassenger(passport)) {
            return;
        }
    });
    dlg.exec();
}

void MainWindow::on_clearPassenger_clicked()
{
    if (QMessageBox::question(this, "Подтверждение",
                              "Очистить всех пассажиров?") == QMessageBox::Yes) {
        ticketingSystem->clearPassengers();
    }
}

void MainWindow::on_displayAllPassenger_clicked()
{
    auto list = ticketingSystem->allPassengers();
    // Если нет ни одного
    if (list.isEmpty()) {
        QMessageBox::information(this, tr("Пассажиры"), tr("Нет ни одного пассажира"));
    }
    // Модель  (паспорт, ФИО)
    auto *model = new QStandardItemModel(list.size(), 2, this);
    model->setHeaderData(0, Qt::Horizontal, tr("Паспорт"));
    model->setHeaderData(1, Qt::Horizontal, tr("ФИО"));
    for (int i = 0; i < list.size(); ++i) {
        const auto &p = list[i];
        model->setItem(i, 0, new QStandardItem(p.GetPassportNumber()));
        model->setItem(i, 1, new QStandardItem(p.GetFullName()));
    }
    ui->passengerTable->setModel(model);
    ui->passengerTable->resizeColumnsToContents();

    // очистить поисковые поля
    ui->passportNumberEdit->clear();
    ui->fullNameEdit->clear();
}
void MainWindow::on_passportNumberEdit_returnPressed()
{
    const QString pass = ui->passportNumberEdit->text().trimmed();
    if (pass.isEmpty()) return;

    QVector<QPair<QString, QString>> flightTickets;
    Passenger *p = ticketingSystem->findPassengerByPassport(pass, flightTickets);
    if (!p) {
        QMessageBox::warning(this, tr("Поиск пассажира"), err->GetLastError());
        ui->passportNumberEdit->clear();
        return;
    }

    // создаём модель: 4 строки под паспортные данные + по одной строке на каждый билет
    int rows = 4 + flightTickets.size();
    auto *model = new QStandardItemModel(rows, 2, this);
    model->setHeaderData(0, Qt::Horizontal, tr("Поле / Рейс"));
    model->setHeaderData(1, Qt::Horizontal, tr("Значение / Билет"));

    // 0–3: информация о пассажире
    model->setItem(0, 0, new QStandardItem(tr("Паспорт")));
    model->setItem(0, 1, new QStandardItem(p->GetPassportNumber()));
    model->setItem(1, 0, new QStandardItem(tr("Дата и место выдачи")));
    model->setItem(1, 1, new QStandardItem(p->GetIssuePlaceAndDate()));
    model->setItem(2, 0, new QStandardItem(tr("ФИО")));
    model->setItem(2, 1, new QStandardItem(p->GetFullName()));
    model->setItem(3, 0, new QStandardItem(tr("Дата рождения")));
    model->setItem(3, 1, new QStandardItem(p->GetBirthDate()));

    // строки 4…: каждый проданный билет
    for (int i = 0; i < flightTickets.size(); ++i) {
        const auto &ft = flightTickets[i];
        model->setItem(4 + i, 0,
                       new QStandardItem(tr("Рейс %1").arg(ft.first)));
        model->setItem(4 + i, 1,
                       new QStandardItem(tr("Билет %1").arg(ft.second)));
    }

    ui->passengerTable->setModel(model);
    ui->passengerTable->resizeColumnsToContents();
    ui->passportNumberEdit->clear();
}

// Поиск пассажира по ФИО:
// выводим таблицу паспорт + ФИО
void MainWindow::on_fullNameEdit_returnPressed()
{
    const QString name = ui->fullNameEdit->text().trimmed();
    if (name.isEmpty()) return;

    auto results = ticketingSystem->findPassengersByName(name);
    if (results.isEmpty()) {
        QMessageBox::warning(this, tr("Поиск пассажиров"), tr("Пассажиров не найдено"));
        ui->fullNameEdit->clear();
        return;
    }

    auto *model = new QStandardItemModel(results.size(), 2, this);
    model->setHeaderData(0, Qt::Horizontal, tr("Паспорт"));
    model->setHeaderData(1, Qt::Horizontal, tr("ФИО"));
    for (int i = 0; i < results.size(); ++i) {
        model->setItem(i, 0, new QStandardItem(results[i].first));
        model->setItem(i, 1, new QStandardItem(results[i].second));
    }

    ui->passengerTable->setModel(model);
    ui->passengerTable->resizeColumnsToContents();

    ui->fullNameEdit->clear();
}



// Рейсы

void MainWindow::on_addFlight_clicked()
{
    FlightDialog dlg(FlightDialog::Add, err, this);
    connect(&dlg, &FlightDialog::added, this, [&](const Flight &f){
        // проверяем, успешно ли прошла валидация внутри конструктора
        if (!f.IsValid()) {
            // ErrorHandler уже показал сообщение
            return;
        }
        if (!ticketingSystem->addFlight(f)) {
            return;
        }
    });

    dlg.exec();
    updateAvailableFlights();
}

void MainWindow::on_removeFlight_clicked()
{
    FlightDialog dlg(FlightDialog::Remove, err, this);
    connect(&dlg, &FlightDialog::removed, this, [&](const QString &num){
        if (!ticketingSystem->removeFlight(num)) {
            return;
        }
    });

    dlg.exec();
    updateAvailableFlights();
}

void MainWindow::on_clearFlight_clicked()
{
    if (QMessageBox::question(this, "Подтверждение",
                              "Очистить все рейсы?") == QMessageBox::Yes) {
        ticketingSystem->clearFlights();
        updateAvailableFlights();
    }
}

void MainWindow::on_displayAllFlight_clicked()
{
    auto list = ticketingSystem->allFlights();
    if (list.isEmpty()) {
        QMessageBox::information(this, tr("Рейсы"), tr("Нет ни одного рейса"));
    }

    auto *model = new QStandardItemModel(list.size(), 5, this);
    model->setHeaderData(0, Qt::Horizontal, tr("Номер"));
    model->setHeaderData(1, Qt::Horizontal, tr("Авиакомпания"));
    model->setHeaderData(2, Qt::Horizontal, tr("Откуда"));
    model->setHeaderData(3, Qt::Horizontal, tr("Куда"));
    model->setHeaderData(4, Qt::Horizontal, tr("Дата/Время"));
    for (int i = 0; i < list.size(); ++i) {
        const auto &f = list[i];
        model->setItem(i, 0, new QStandardItem(f.GetFlightNumber()));
        model->setItem(i, 1, new QStandardItem(f.GetAirlineName()));
        model->setItem(i, 2, new QStandardItem(f.GetDepartureAirport()));
        model->setItem(i, 3, new QStandardItem(f.GetArrivalAirport()));
        model->setItem(i, 4, new QStandardItem(f.GetDepartureDate() + " " + f.GetDepartureTime()));
    }
    ui->flightTable->setModel(model);
    ui->flightTable->resizeColumnsToContents();

    ui->flightNumberEdit->clear();
    ui->destinationEdit->clear();
}

void MainWindow::on_flightNumberEdit_returnPressed()
{
    const QString num = ui->flightNumberEdit->text().trimmed();
    if (num.isEmpty()) return;

    QVector<QPair<QString,QString>> passengers;
    Flight *f = ticketingSystem->findFlightByNumber(num, passengers);
    if (!f) {
        ui->flightNumberEdit->clear();
        return;
    }

    // Модель: свед. о рейсе (6 строк) + список пассажиров
    int rowCount = 6 + passengers.size();
    auto *model = new QStandardItemModel(rowCount, 2, this);
    model->setHeaderData(0, Qt::Horizontal, tr("Поле/Пассажир"));
    model->setHeaderData(1, Qt::Horizontal, tr("Значение/ФИО"));

    // Строки 0–5: данные рейса
    model->setItem(0, 0, new QStandardItem(tr("Номер рейса")));
    model->setItem(0, 1, new QStandardItem(f->GetFlightNumber()));
    model->setItem(1, 0, new QStandardItem(tr("Авиакомпания")));
    model->setItem(1, 1, new QStandardItem(f->GetAirlineName()));
    model->setItem(2, 0, new QStandardItem(tr("Откуда")));
    model->setItem(2, 1, new QStandardItem(f->GetDepartureAirport()));
    model->setItem(3, 0, new QStandardItem(tr("Куда")));
    model->setItem(3, 1, new QStandardItem(f->GetArrivalAirport()));
    model->setItem(4, 0, new QStandardItem(tr("Дата отправления")));
    model->setItem(4, 1, new QStandardItem(f->GetDepartureDate()));
    model->setItem(5, 0, new QStandardItem(tr("Время отправления")));
    model->setItem(5, 1, new QStandardItem(f->GetDepartureTime()));

    // Строки 6…: пассажиры
    for (int i = 0; i < passengers.size(); ++i) {
        model->setItem(i + 6, 0, new QStandardItem(passengers[i].first));  // паспорт
        model->setItem(i + 6, 1, new QStandardItem(passengers[i].second)); // ФИО
    }

    ui->flightTable->setModel(model);
    ui->flightTable->resizeColumnsToContents();

    ui->flightNumberEdit->clear();
}

// Поиск рейсов по фрагменту аэропорта прибытия:
// выводим таблицу: Номер, Прибытие, Дата, Время
void MainWindow::on_destinationEdit_returnPressed()
{
    const QString frag = ui->destinationEdit->text().trimmed();
    if (frag.isEmpty()) return;

    auto list = ticketingSystem->findFlightsByArrivalFragment(frag);
    if (list.isEmpty()) {
        ui->destinationEdit->clear();
        return;
    }

    auto *model = new QStandardItemModel(list.size(), 4, this);
    model->setHeaderData(0, Qt::Horizontal, tr("Номер"));
    model->setHeaderData(1, Qt::Horizontal, tr("Аэропорт прибытия"));
    model->setHeaderData(2, Qt::Horizontal, tr("Дата"));
    model->setHeaderData(3, Qt::Horizontal, tr("Время"));
    for (int i = 0; i < list.size(); ++i) {
        const auto &f = list[i];
        model->setItem(i, 0, new QStandardItem(f.GetFlightNumber()));
        model->setItem(i, 1, new QStandardItem(f.GetArrivalAirport()));
        model->setItem(i, 2, new QStandardItem(f.GetDepartureDate()));
        model->setItem(i, 3, new QStandardItem(f.GetDepartureTime()));
    }

    ui->flightTable->setModel(model);
    ui->flightTable->resizeColumnsToContents();

    ui->destinationEdit->clear();
}

// Билеты

void MainWindow::on_sellTicket_clicked()
{
    TicketDialog dlg(TicketDialog::Sell, this);
    connect(&dlg, &TicketDialog::sold, this, [&](const QString &pass,
                                                 const QString &flight,
                                                 const QString &ticket){
        if (!ticketingSystem->sellTicket(pass, flight, ticket)) {
        }
    });

    dlg.exec();
    updateAvailableFlights();
}

void MainWindow::on_returnTicket_clicked()
{
    TicketDialog dlg(TicketDialog::Return, this);
    connect(&dlg, &TicketDialog::returned, this, [&](const QString &ticket){
        if (!ticketingSystem->returnTicket(ticket)) {
        }
    });

    dlg.exec();
    updateAvailableFlights();
}
// Создание новой БД (пустого файла)
void MainWindow::on_createBD_clicked()
{
    QString file = QFileDialog::getSaveFileName(this,
                                                tr("Создать базу данных"), QString(),
                                                tr("json файлы (*.json);;Все файлы (*)"));
    if (file.isEmpty()) return;

    if (!db->createNew(file)) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось создать файл базы"));
        return;
    }
    QMessageBox::information(this, tr("Готово"), tr("База данных создана"));
    ui->currentBD->setText(QString("Текущая база данных: %1").arg(db->currentFileName()));
    updateUiEnabled();
    updateAvailableFlights();
}

// Загрузка (открытие) существующей БД
void MainWindow::on_uploadBD_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,
                                                tr("Загрузить базу данных"), QString(),
                                                tr("json файлы (*.json);;Все файлы (*)"));
    if (file.isEmpty()) return;

    if (!db->load(file)) {
        QMessageBox::warning(this, tr("Ошибка"), err->GetLastError());
        return;
    }
    QMessageBox::information(this, tr("Готово"), tr("База данных загружена"));
    ui->currentBD->setText(QString("Текущая база данных: %1").arg(db->currentFileName()));
    updateUiEnabled();
    updateAvailableFlights();

}

// Удаление файла БД
void MainWindow::on_deleteBD_clicked()
{
    if (!(db->isCurrentFilePathSet())) {
        QMessageBox::warning(this, tr("Ошибка"), tr("База данных не выбрана"));
        return;
    }
    if (QMessageBox::question(this, tr("Подтверждение"),
                              tr("Удалить файл базы \"%1\"?").arg(db->currentFileName()))
        != QMessageBox::Yes)
        return;

    if (!db->removeFile(db->getCurrentFilePath())) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось удалить файл"));
        return;
    }
    QMessageBox::information(this, tr("Готово"), tr("Файл базы удалён"));
    ui->currentBD->setText(QString("Текущая база данных: отсутствует"));
    updateUiEnabled();
    updateAvailableFlights();
}

// Создание копии текущей БД
void MainWindow::on_copyBD_clicked()
{
    if (!(db->isCurrentFilePathSet())) {
        QMessageBox::warning(this, tr("Ошибка"), tr("База данных не выбрана"));
        return;
    }
    QString file = QFileDialog::getSaveFileName(this,
                                                tr("Сохранить копию базы"), QString(),
                                                tr("json файлы (*.json);;Все файлы (*)"));
    if (file.isEmpty()) return;

    if (!db->copy(db->getCurrentFilePath(), file)) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось создать копию"));
        return;
    }
    QMessageBox::information(this, tr("Готово"),
                             tr("Копия базы сохранена в \"%1\"").arg(file));
    ui->currentBD->setText(QString("Текущая база данных: %1").arg(db->currentFileName()));
}

void MainWindow::on_saveDB_clicked()
{
    // Получаем текущий путь из объекта Database
    const QString path = db->getCurrentFilePath();
    if (path.isEmpty()) {
        // Если файл ещё не выбран или не был создан
        QMessageBox::warning(this, tr("Сохранение базы"), tr("Текущая база данных не задана"));
        return;
    }

    // Пытаемся сохранить
    if (!db->save(path)) {
        // При неудаче показываем ошибку из ErrorHandler или общее сообщение
        const QString errMsg = err->GetLastError().isEmpty()
                                   ? tr("Не удалось сохранить базу данных")
                                   : err->GetLastError();
        QMessageBox::critical(this, tr("Сохранение базы"), errMsg);
        return;
    }

    // Успешно
    QMessageBox::information(this, tr("Сохранение базы"), tr("База данных сохранена"));
}

