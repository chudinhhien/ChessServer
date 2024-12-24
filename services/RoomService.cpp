#include "RoomService.h"

RoomService::RoomService(RoomRepository *repository) : repository(repository) {}

bool RoomService::createRoom(const QString &username, QString &roomId, QString &message) {
    if (username.isEmpty()) {
        message = "Username cannot be empty.";
        return false;
    }

    QString errorMessage;
    if (!repository->createRoom(username, roomId, errorMessage)) {
        message = "Failed to create room: " + errorMessage;
        return false;
    }

    message = "Room created successfully.";
    return true;
}
