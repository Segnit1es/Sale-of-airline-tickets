#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QFile>
#include <QDataStream>
#include <QFileInfo>
#include <memory>
#include "ticketingsystem.h"

class Database {
public:
    explicit Database(TicketingSystem *ts);
    ~Database();

    // Создать новую БД
    bool createNew(const QString &filePath);

    // Сохранить текущие данные системы в файл
    bool save(const QString &filePath);

    // Загрузить данные из файла, заменив текущее состояние
    bool load(const QString &filePath);

    // Удалить файл БД
    bool removeFile(const QString &filePath);

    // Создать копию существующей БД
    bool copy(const QString &srcPath, const QString &dstPath);

    void setCurrentFilePath(const QString &path);
    QString getCurrentFilePath() const;
    bool isCurrentFilePathSet() const {
        return !currentFilePath.isEmpty();
    }
    QString currentFileName() const;

private:
    TicketingSystem *ts_;  // не владеем, только взаимодействуем
    QString currentFilePath;
};

#endif // DATABASE_H
