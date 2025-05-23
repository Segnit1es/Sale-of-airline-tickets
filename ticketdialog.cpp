#include "ticketdialog.h"
#include "qheaderview.h"
#include <QStandardItemModel>

TicketDialog::TicketDialog(Mode mode, QWidget *parent)
    : QDialog(parent), m_mode(mode)
{
    setupUi();
    connect(m_okButton, &QPushButton::clicked, this, [this]() {
        switch (m_mode) {
        case Sell:
            emit sold(passportNumber(), flightNumber(), ticketNumber());
            break;
        case Return:
            emit returned(ticketNumber());
            break;
        }
        accept();
    });
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void TicketDialog::setupUi()
{
    m_mainLayout   = new QVBoxLayout(this);
    m_okButton     = new QPushButton(tr("OK"), this);
    m_cancelButton = new QPushButton(tr("Cancel"), this);

    switch (m_mode) {
    case Sell:                initSellUi();    break;
    case Return:              initReturnUi();  break;
    }

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(m_okButton);
    btnLayout->addWidget(m_cancelButton);
    m_mainLayout->addLayout(btnLayout);
}

void TicketDialog::initSellUi()
{
    setWindowTitle(tr("Продать билет"));
    m_mainLayout->addWidget(new QLabel(tr("Паспорт пассажира:"), this));
    m_passportEdit = new QLineEdit(this);
    m_mainLayout->addWidget(m_passportEdit);

    m_mainLayout->addWidget(new QLabel(tr("Номер рейса:"), this));
    m_flightEdit = new QLineEdit(this);
    m_mainLayout->addWidget(m_flightEdit);

    m_mainLayout->addWidget(new QLabel(tr("Номер билета (9 цифр):"), this));
    m_ticketEdit = new QLineEdit(this);
    m_mainLayout->addWidget(m_ticketEdit);
}

void TicketDialog::initReturnUi()
{
    setWindowTitle(tr("Возврат билета"));
    m_mainLayout->addWidget(new QLabel(tr("Номер билета:"), this));
    m_ticketEdit = new QLineEdit(this);
    m_mainLayout->addWidget(m_ticketEdit);
}


QString TicketDialog::passportNumber() const
{
    return m_passportEdit ? m_passportEdit->text().trimmed() : QString();
}

QString TicketDialog::flightNumber() const
{
    return m_flightEdit ? m_flightEdit->text().trimmed() : QString();
}

QString TicketDialog::ticketNumber() const
{
    return m_ticketEdit ? m_ticketEdit->text().trimmed() : QString();
}


