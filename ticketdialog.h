// TicketDialog.h
#ifndef TICKETDIALOG_H
#define TICKETDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "ticketrecord.h"

class TicketDialog : public QDialog {
    Q_OBJECT
public:
    enum Mode {
        Sell,
        Return
    };

    explicit TicketDialog(Mode mode, QWidget *parent = nullptr);

    QString passportNumber() const;
    QString flightNumber() const;
    QString ticketNumber() const;

signals:
    void sold(const QString &passport, const QString &flight, const QString &ticket);
    void returned(const QString &ticket);

private:
    Mode m_mode;
    QVBoxLayout *m_mainLayout;
    QLineEdit   *m_passportEdit = nullptr;
    QLineEdit   *m_flightEdit   = nullptr;
    QLineEdit   *m_ticketEdit   = nullptr;
    QTableView  *m_tableView    = nullptr;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;

    void setupUi();
    void initSellUi();
    void initReturnUi();

};

#endif // TICKETDIALOG_H
