#include "RoomRepository.h"

RoomRepository::RoomRepository(QSqlDatabase &db) : db(db) {}

bool RoomRepository::createRoom(const QString &username, QString &roomId, QString &errorMessage) {
    roomId = QUuid::createUuid().toString(QUuid::WithoutBraces); // Tạo roomId duy nhất

    QSqlQuery query(db);
    query.prepare("INSERT INTO rooms (room_id, owner_username) VALUES (:room_id, :owner_username)");
    query.bindValue(":room_id", roomId);
    query.bindValue(":owner_username", username);

    if (!query.exec()) {
        errorMessage = query.lastError().text();
        qWarning() << "Failed to create room:" << errorMessage;
        return false;
    }

    return true;
}
