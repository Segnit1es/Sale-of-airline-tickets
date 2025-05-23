#ifndef PASSENGERSDIALOG_H
#define PASSENGERSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include "passenger.h"

class PassengersDialog : public QDialog {
    Q_OBJECT
public:
    enum Mode {
        Register,
        Remove
    };

    explicit PassengersDialog(Mode mode, ErrorHandler *err, QWidget *parent = nullptr);

    // Геттеры для доступа к виджетам
    QLineEdit* passportNumberEdit() const { return m_passportEdit; }
    QLineEdit* issuePlaceEdit()     const { return m_issuePlaceEdit; }
    QDateEdit* birthDateEdit()      const { return m_birthDateEdit; }
    QLineEdit* fullNameEdit()       const { return m_fullNameEdit; }
    QTableView* tableView()         const { return m_tableView; }

    QString passportNumber() const;
    QString issuePlaceAndDate() const;
    QString birthDate() const;
    QString fullName() const;

signals:
    void registered(const QString &passport,
                    const QString &issuePlace,
                    const QString &name,
                    const QString &birthDate);
    void removed(const QString &passport);


private:
    Mode         m_mode;
    ErrorHandler   *errorHandler;
    QVBoxLayout *m_mainLayout;
    QLineEdit   *m_passportEdit     = nullptr;
    QLineEdit   *m_issuePlaceEdit   = nullptr;
    QDateEdit   *m_birthDateEdit    = nullptr;
    QLineEdit   *m_fullNameEdit     = nullptr;
    QTableView  *m_tableView        = nullptr;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;

    void setupUi();
    void initRegisterUi();
    void initRemoveUi();

};

#endif // PASSENGERSDIALOG_H

