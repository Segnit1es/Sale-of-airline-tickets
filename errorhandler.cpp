#include "errorhandler.h"
#include <QMessageBox>

ErrorHandler::ErrorHandler(QObject *parent)
    : QObject(parent)
{}

void ErrorHandler::RaiseError(const QString &message, const QString &source)
{
    QString fullMessage = source.isEmpty()
    ? message
    : QString("[%1] %2").arg(source, message);
    lastError = fullMessage;
    qWarning() << fullMessage;

    // Показываем модальное окно об ошибке
    QMessageBox::critical(nullptr, tr("Ошибка"), fullMessage);

    emit ErrorOccurred(fullMessage);
}

QString ErrorHandler::GetLastError() const
{
    return lastError;
}
