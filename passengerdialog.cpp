#include "passengerdialog.h"
#include <QHBoxLayout>
#include <QLabel>
#include "errorhandler.h"
#include "qheaderview.h"

PassengersDialog::PassengersDialog(Mode mode, ErrorHandler *err, QWidget *parent)
    : QDialog(parent), m_mode(mode), errorHandler(err)
{
    setupUi();
    connect(m_okButton, &QPushButton::clicked, this, [this]() {
        switch (m_mode) {
        case Register:
            emit registered(
                passportNumber(),
                issuePlaceAndDate(),
                fullName(),
                birthDate()
                );
            break;
        case Remove:
            emit removed(passportNumber());
            break;
        }
        accept();
    });
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void PassengersDialog::setupUi()
{
    m_mainLayout = new QVBoxLayout(this);
    m_okButton   = new QPushButton(tr("OK"), this);
    m_cancelButton = new QPushButton(tr("Отмена"), this);

    switch (m_mode) {
    case Register:         initRegisterUi();       break;
    case Remove:           initRemoveUi();         break;
    }

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(m_okButton);
    btnLayout->addWidget(m_cancelButton);
    m_mainLayout->addLayout(btnLayout);
}

void PassengersDialog::initRegisterUi()
{
    setWindowTitle(tr("Регистрация пассажира"));

    // ФИО
    m_mainLayout->addWidget(new QLabel(tr("ФИО:"), this));
    m_fullNameEdit = new QLineEdit(this);
    m_mainLayout->addWidget(m_fullNameEdit);

    // Место и дата выдачи
    m_mainLayout->addWidget(new QLabel(tr("Место и дата выдачи:"), this));
    m_issuePlaceEdit = new QLineEdit(this);
    m_issuePlaceEdit->setPlaceholderText(tr("Город, дата (ДД.MM.ГГГГ)"));
    m_mainLayout->addWidget(m_issuePlaceEdit);

    // Дата рождения
    m_mainLayout->addWidget(new QLabel(tr("Дата рождения:"), this));
    m_birthDateEdit = new QDateEdit(QDate::currentDate(), this);
    m_birthDateEdit->setDisplayFormat("dd.MM.yyyy");
    m_birthDateEdit->setCalendarPopup(true);
    m_mainLayout->addWidget(m_birthDateEdit);

    // Номер паспорта
    m_mainLayout->addWidget(new QLabel(tr("Номер паспорта:"), this));
    m_passportEdit = new QLineEdit(this);
    m_passportEdit->setInputMask("0000-000000");
    m_mainLayout->addWidget(m_passportEdit);
}

void PassengersDialog::initRemoveUi()
{
    setWindowTitle(tr("Удаление пассажира"));
    m_mainLayout->addWidget(new QLabel(tr("Номер паспорта:"), this));
    m_passportEdit = new QLineEdit(this);
    m_passportEdit->setInputMask("0000-000000");
    m_mainLayout->addWidget(m_passportEdit);
}

QString PassengersDialog::passportNumber() const
{
    return m_passportEdit ? m_passportEdit->text().trimmed() : QString();
}

QString PassengersDialog::issuePlaceAndDate() const
{
    return m_issuePlaceEdit ? m_issuePlaceEdit->text().trimmed() : QString();
}

QString PassengersDialog::birthDate() const
{
    return m_birthDateEdit ? m_birthDateEdit->date().toString("dd.MM.yyyy") : QString();
}

QString PassengersDialog::fullName() const
{
    return m_fullNameEdit ? m_fullNameEdit->text().trimmed() : QString();
}


