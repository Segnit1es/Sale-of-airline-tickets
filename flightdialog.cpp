#include "flightdialog.h"
#include "qheaderview.h"
#include <QStandardItemModel>
#include <QHBoxLayout>
#include <QLabel>

FlightDialog::FlightDialog(Mode mode, ErrorHandler *err, QWidget *parent)
    : QDialog(parent), m_mode(mode), errorHandler(err)
{
    setupUi();
    connect(m_okButton, &QPushButton::clicked, this, [this]() {
        switch (m_mode) {
        case Add: {
            Flight f(
                m_numberEdit->text().trimmed(),
                m_airlineEdit->text().trimmed(),
                m_depAirportEdit->text().trimmed(),
                m_arrAirportEdit->text().trimmed(),
                m_dateEdit->date().toString("dd.MM.yyyy"),
                m_timeEdit->text().trimmed(),
                m_totalSeatsEdit->text().toInt(),
                m_freeSeatsEdit->text().toInt(),
                errorHandler
                );
            emit added(f);
            break;
        }
        case Remove:
            emit removed(m_numberEdit->text().trimmed());
            break;
        }
        accept();
    });
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void FlightDialog::setupUi()
{
    m_mainLayout   = new QVBoxLayout(this);
    m_okButton     = new QPushButton(tr("OK"), this);
    m_cancelButton = new QPushButton(tr("Cancel"), this);

    switch (m_mode) {
    case Add:              initAddUi();              break;
    case Remove:           initRemoveUi();           break;
    }

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(m_okButton);
    btnLayout->addWidget(m_cancelButton);
    m_mainLayout->addLayout(btnLayout);
}

void FlightDialog::initAddUi()
{
    setWindowTitle(tr("Добавить рейс"));
    auto addLabel = [&](const QString &text, QWidget *w){
        m_mainLayout->addWidget(new QLabel(text, this));
        m_mainLayout->addWidget(w);
    };

    m_numberEdit     = new QLineEdit(this);
    addLabel(tr("Номер рейса (AAA-NNN):"), m_numberEdit);

    m_airlineEdit    = new QLineEdit(this);
    addLabel(tr("Авиакомпания:"), m_airlineEdit);

    m_depAirportEdit = new QLineEdit(this);
    addLabel(tr("Аэропорт отправления:"), m_depAirportEdit);

    m_arrAirportEdit = new QLineEdit(this);
    addLabel(tr("Аэропорт прибытия:"), m_arrAirportEdit);

    m_dateEdit       = new QDateEdit(QDate::currentDate(), this);
    m_dateEdit->setDisplayFormat("dd.MM.yyyy");
    addLabel(tr("Дата отправления:"), m_dateEdit);

    m_timeEdit       = new QLineEdit(this);
    addLabel(tr("Время отправления (HH:MM):"), m_timeEdit);

    m_totalSeatsEdit = new QLineEdit(this);
    addLabel(tr("Всего мест:"), m_totalSeatsEdit);

    m_freeSeatsEdit  = new QLineEdit(this);
    addLabel(tr("Свободных мест:"), m_freeSeatsEdit);
}

void FlightDialog::initRemoveUi()
{
    setWindowTitle(tr("Удалить рейс"));
    m_mainLayout->addWidget(new QLabel(tr("Номер рейса:"), this));
    m_numberEdit = new QLineEdit(this);
    m_mainLayout->addWidget(m_numberEdit);
}
