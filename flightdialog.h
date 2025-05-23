#ifndef FLIGHTDIALOG_H
#define FLIGHTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDateEdit>
#include <memory>
#include "flight.h"
#include "errorhandler.h"

class FlightDialog : public QDialog {
    Q_OBJECT
public:
    enum Mode {
        Add,
        Remove
    };

    explicit FlightDialog(Mode mode, ErrorHandler *err, QWidget *parent = nullptr);

    // Геттеры для полей ввода (обратите внимание: возвращают QLineEdit* и QDateEdit*)
    QLineEdit* flightNumberEdit()    const { return m_numberEdit; }
    QLineEdit* airlineNameEdit()     const { return m_airlineEdit; }
    QLineEdit* departureAirportEdit() const { return m_depAirportEdit; }
    QLineEdit* arrivalAirportEdit()   const { return m_arrAirportEdit; }
    QDateEdit* departureDateEdit()    const { return m_dateEdit; }
    QLineEdit* departureTimeEdit()    const { return m_timeEdit; }
    QLineEdit* totalSeatsEdit()       const { return m_totalSeatsEdit; }
    QLineEdit* freeSeatsEdit()        const { return m_freeSeatsEdit; }
    QTableView* tableView()           const { return m_tableView; }

signals:
    void added(const Flight &f);
    void removed(const QString &flightNumber);

private:
    Mode                            m_mode;
    ErrorHandler   *errorHandler;
    QVBoxLayout                    *m_mainLayout;
    QLineEdit                      *m_numberEdit       = nullptr;
    QLineEdit                      *m_airlineEdit      = nullptr;
    QLineEdit                      *m_depAirportEdit   = nullptr;
    QLineEdit                      *m_arrAirportEdit   = nullptr;
    QDateEdit                      *m_dateEdit         = nullptr;
    QLineEdit                      *m_timeEdit         = nullptr;
    QLineEdit                      *m_totalSeatsEdit   = nullptr;
    QLineEdit                      *m_freeSeatsEdit    = nullptr;
    QTableView                     *m_tableView        = nullptr;
    QPushButton                    *m_okButton;
    QPushButton                    *m_cancelButton;

    void setupUi();
    void initAddUi();
    void initRemoveUi();
};

#endif // FLIGHTDIALOG_H

