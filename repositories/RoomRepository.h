#ifndef ROOMREPOSITORY_H
#define ROOMREPOSITORY_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>
#include <QDebug>

class RoomRepository {
public:
    explicit RoomRepository(QSqlDatabase &db);

    bool createRoom(const QString &username, QString &roomId, QString &errorMessage);

private:
    QSqlDatabase db;
};

#endif // ROOMREPOSITORY_H
