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
