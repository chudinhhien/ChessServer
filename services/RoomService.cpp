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

    if (accepted) {
        // Tạo trận đấu
        QString matchId = matchService->createMatch(fromPlayer, toPlayer, fromSocket, toSocket);
        authService->updatePlayerStatus(fromSocket, "In Match");
        authService->updatePlayerStatus(toSocket, "In Match");

        // Gửi thông báo bắt đầu trận đấu đến cả hai người chơi
        QJsonObject matchStartMessage;
        matchStartMessage["type"] = "match_start";
        matchStartMessage["match_id"] = matchId;

        QJsonDocument doc(matchStartMessage);
        fromSocket->write(doc.toJson());
        fromSocket->flush();

        toSocket->write(doc.toJson());
        toSocket->flush();

        qDebug() << "Match started between" << fromPlayer << "and" << toPlayer;
    } else {
        // Gửi thông báo từ chối đến người mời
        QJsonObject declineMessage;
        declineMessage["type"] = "invite_decline";
        declineMessage["from"] = toPlayer;

        QJsonDocument doc(declineMessage);
        fromSocket->write(doc.toJson());
        fromSocket->flush();

        qDebug() << toPlayer << "declined invite from" << fromPlayer;
    }
}

