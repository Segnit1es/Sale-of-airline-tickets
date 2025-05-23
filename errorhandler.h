#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <QObject>
#include <QString>
#include <QDebug>

class ErrorHandler : public QObject {
    Q_OBJECT

public:
    explicit ErrorHandler(QObject *parent = nullptr);

    void RaiseError(const QString &message, const QString &source = QString());
    QString GetLastError() const;

signals:
    void ErrorOccurred(const QString &fullMessage);

private:
    QString lastError;
};

#endif // ERRORHANDLER_H
