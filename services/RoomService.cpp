#include "RoomService.h"
#include "QJsonObject"
#include "QJsonDocument"

RoomService::RoomService(RoomRepository *repository, AuthService *authService, MatchService *matchService)
    : repository(repository), authService(authService), matchService(matchService) {}

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

void RoomService::sendInvite(const QString &fromPlayer, const QString &toPlayer, QTcpSocket *fromSocket, QTcpSocket *toSocket, const QString &fromPlayerName) {
    // Gửi lời mời đến người chơi B
    QJsonObject inviteMessage;
    inviteMessage["type"] = "invite_player_ack";
    inviteMessage["username"] = fromPlayer;
    inviteMessage["name"] = fromPlayerName;

    QJsonDocument doc(inviteMessage);
    toSocket->write(doc.toJson());
    toSocket->flush();

    // Lưu trạng thái lời mời đang chờ
    pendingInvites[toPlayer] = fromPlayer;

    // Phản hồi người gửi A
    QJsonObject response;
    // response["type"] = "invite_ack";
    // response["status"] = "success";
    // QJsonDocument ackDoc(response);
    // fromSocket->write(ackDoc.toJson());
    // fromSocket->flush();

    qDebug() << "Invite sent from" << fromPlayer << "to" << toPlayer;
}

void RoomService::handleInviteResponse(const QString &toPlayer, bool accepted) {
    // Lấy người mời từ pendingInvites
    QString fromPlayer = pendingInvites.value(toPlayer);
    pendingInvites.remove(toPlayer);

    // Sử dụng AuthService để lấy socket của người mời và người được mời
    QTcpSocket *fromSocket = authService->getSocketByUserName(fromPlayer);
    QTcpSocket *toSocket = authService->getSocketByUserName(toPlayer);

    // Kiểm tra nếu không tìm thấy socket
    if (!fromSocket || !toSocket) {
        qDebug() << "Error: Unable to find sockets for players:" << fromPlayer << "or" << toPlayer;
        return;
    }

    // Tạo thông điệp phản hồi
    QJsonObject responseMessage;
    responseMessage["type"] = "respond_invite_ack";

    if (accepted) {
        // Cập nhật trạng thái của cả hai người chơi
        authService->updatePlayerStatus(fromSocket, "In Room");
        authService->updatePlayerStatus(toSocket, "In Room");

        // Lấy thông tin người được mời từ AuthService
        User toUser = authService->getUserInfo(toPlayer);

        // Tạo phản hồi
        QJsonObject userObject;
        userObject["name"] = toUser.getName();
        userObject["username"] = toUser.getUsername();
        userObject["elo"] = toUser.getElo();
        userObject["state"] = toUser.getState();

        responseMessage["status"] = "success";
        responseMessage["message"] = "Invite accepted.";
        responseMessage["user"] = userObject;

        // Gửi phản hồi đến người mời
        QJsonDocument doc(responseMessage);
        fromSocket->write(doc.toJson());
        fromSocket->flush();

        qDebug() << "Invite accepted by" << toPlayer << "from" << fromPlayer;
    } else {
        // Trường hợp từ chối lời mời
        responseMessage["status"] = "failure";
        responseMessage["message"] = "Invite declined.";

        // Gửi phản hồi đến người mời
        QJsonDocument doc(responseMessage);
        fromSocket->write(doc.toJson());
        fromSocket->flush();

        qDebug() << "Invite declined by" << toPlayer << "from" << fromPlayer;
    }
}
