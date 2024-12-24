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

    //pen[B] = A

    // Phản hồi người gửi A
    QJsonObject response;
    // response["type"] = "invite_ack";
    // response["status"] = "success";
    // QJsonDocument ackDoc(response);
    // fromSocket->write(ackDoc.toJson());
    // fromSocket->flush();

    qDebug() << "Invite sent from" << fromPlayer << "to" << toPlayer;
}

void RoomService::handleInviteResponse(const QString &fromPlayer, bool accepted) {
    // Lấy người được mời từ pendingInvites
    QString toPlayer = pendingInvites.value(fromPlayer);
    pendingInvites.remove(fromPlayer);

    // Sử dụng AuthService để lấy socket của người mời và người được mời
    QTcpSocket *fromSocket = authService->getSocketByUserName(fromPlayer);
    QTcpSocket *toSocket = authService->getSocketByUserName(toPlayer);

    // Kiểm tra nếu không tìm thấy socket
    if (!fromSocket || !toSocket) {
        qDebug() << "Error: Unable to find sockets for players:" << fromPlayer << "or" << toPlayer;
        return;
    }

    // Tạo thông điệp phản hồi
    QJsonObject responseMessage, responseMessageRepond;
    responseMessage["type"] = "invitation_ack";
    responseMessageRepond["type"] = "respond_invite_ack";

    if (accepted) {
        // Cập nhật trạng thái của cả hai người chơi
        authService->updatePlayerStatus(fromSocket, "In Room");
        authService->updatePlayerStatus(toSocket, "In Room");

        // Lấy thông tin người được mời từ AuthService
        User toUser = authService->getUserInfo(toPlayer);
        User fromUser = authService->getUserInfo(fromPlayer);

        // Tạo phản hồi cho người mời
        QJsonObject userObject;
        userObject["name"] = fromUser.getName();
        userObject["username"] = fromUser.getUsername();
        userObject["elo"] = fromUser.getElo();
        userObject["state"] = fromUser.getState(); // Trạng thái mới

        responseMessage["status"] = "success";
        responseMessage["message"] = "Invite accepted.";
        responseMessage["user"] = userObject;

        userObject["name"] = toUser.getName();
        userObject["username"] = toUser.getUsername();
        userObject["elo"] = toUser.getElo();
        userObject["state"] = toUser.getState(); // Trạng thái mới

        // Tạo phản hồi cho người được mời
        responseMessageRepond["status"] = "success";
        responseMessageRepond["message"] = "You have joined the room.";
        responseMessageRepond["user"] = userObject;

        // Gửi phản hồi đến người mời
        QJsonDocument docFrom(responseMessage);
        toSocket->write(docFrom.toJson());
        toSocket->flush();

        // Gửi phản hồi đến người được mời
        QJsonDocument docTo(responseMessageRepond);
        fromSocket->write(docTo.toJson());
        fromSocket->flush();

        qDebug() << "Invite accepted by" << toPlayer << "from" << fromPlayer;
    } else {
        // Trường hợp từ chối lời mời
        responseMessage["status"] = "failure";
        responseMessage["message"] = "Invite declined.";

        responseMessageRepond["status"] = "failure";
        responseMessageRepond["message"] = "You have declined the invite.";

        // Gửi phản hồi đến người mời
        QJsonDocument docFrom(responseMessage);
        fromSocket->write(docFrom.toJson());
        fromSocket->flush();

        // Gửi phản hồi đến người được mời
        QJsonDocument docTo(responseMessageRepond);
        toSocket->write(docTo.toJson());
        toSocket->flush();

        qDebug() << "Invite declined by" << toPlayer << "from" << fromPlayer;
    }
}

QString RoomService::createMatch(const QString &player1, const QString &player2) {
    QTcpSocket* player1Socket = authService->getSocketByUserName(player1);
    QTcpSocket* player2Socket = authService->getSocketByUserName(player2);
    QString matchId = matchService->createMatch(player1, player2, player1Socket , player2Socket);
    QJsonObject response1;
    response1["type"] = "start_game_ack";
    response1["status"] = "success";
    response1["match_id"] = matchId;
    response1["opponent"] = player2;
    response1["role"] = "WHITE";
    response1["message"] = "Match created successfully.";
    player1Socket->write(QJsonDocument(response1).toJson());
    player1Socket->flush();

    // Gửi phản hồi thành công cho người chơi thứ hai
    QJsonObject response2;
    response2["type"] = "start_game_ack";
    response2["status"] = "success";
    response2["match_id"] = matchId;
    response2["opponent"] = player1;
    response2["role"] = "BLACK";
    response2["message"] = "Match created successfully.";
    player2Socket->write(QJsonDocument(response2).toJson());
    player2Socket->flush();

    qDebug() << "Match created with ID:" << matchId
             << "Player1:" << player1
             << "Player2:" << player2;
    return matchId;
}
