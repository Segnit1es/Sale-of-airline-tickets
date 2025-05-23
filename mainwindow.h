#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "errorhandler.h"
#include "database.h"

namespace Ui {
class MainWindow;
}

class TicketingSystem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Пассажиры
    void on_addPassenger_clicked();
    void on_removePassenger_clicked();
    void on_displayAllPassenger_clicked();
    void on_clearPassenger_clicked();
    void on_passportNumberEdit_returnPressed();
    void on_fullNameEdit_returnPressed();

    // Рейсы
    void on_addFlight_clicked();
    void on_removeFlight_clicked();
    void on_displayAllFlight_clicked();
    void on_clearFlight_clicked();
    void on_flightNumberEdit_returnPressed();
    void on_destinationEdit_returnPressed();

    // Билеты
    void on_sellTicket_clicked();
    void on_returnTicket_clicked();

    void on_createBD_clicked();
    void on_uploadBD_clicked();
    void on_deleteBD_clicked();
    void on_copyBD_clicked();
    void on_saveDB_clicked();

private:
    void updateUiEnabled();
    void updateAvailableFlights();
    Ui::MainWindow              *ui;
    ErrorHandler *err;                   // общая обработка ошибок
    std::unique_ptr<TicketingSystem> ticketingSystem;      // бизнес-логика
    std::unique_ptr<Database> db;        // менеджер файловой БД
};

#endif // MAINWINDOW_H
