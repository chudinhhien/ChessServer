#include "RoomController.h"
#include "QJsonDocument"

RoomController::RoomController(RoomService *service, AuthService *authService, QObject *parent)
    : QObject(parent), service(service), authService(authService) {}

void RoomController::handleCreateRoom(QTcpSocket *client, const QString &username) {
    QString roomId, message;
    QJsonObject response;
    response["type"] = "create_room_ack";

    if (service->createRoom(username, roomId, message)) {
        authService->updatePlayerStatus(client, "In Room");
        response["status"] = "success";
        response["message"] = message;
        response["room_id"] = roomId;
    } else {
        response["status"] = "failure";
        response["message"] = message;
    }

    QJsonDocument doc(response);
    client->write(doc.toJson());
    client->flush();

    qDebug() << "Create room response sent to client:" << response;
}

void RoomController::handleInvite(QTcpSocket *client, const QString &fromPlayer, const QString &toPlayer, const QString &fromPlayerName) {
    // Lấy socket của người được mời (toPlayer)
    QTcpSocket *toSocket = authService->getSocketByUserName(toPlayer);

    if (toSocket) {
        service->sendInvite(fromPlayer, toPlayer, client, toSocket, fromPlayerName);
    } else {
        // Người được mời không có socket (trường hợp ngoại lệ)
        QJsonObject response;
        response["type"] = "invite_ack";
        response["status"] = "failure";
        response["message"] = "Player not available.";
        QJsonDocument doc(response);
        client->write(doc.toJson());
        client->flush();
    }
}

void RoomController::handleInviteResponse(QTcpSocket *client, const QString &toPlayer, bool accepted) {
    service->handleInviteResponse(toPlayer, accepted);
}
