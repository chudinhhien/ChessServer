#include "MatchController.h"
#include <QJsonDocument>
#include <QJsonObject>

MatchController::MatchController(MatchService *service, QObject *parent) : QObject(parent), service(service) {}

void MatchController::handleFindMatch(QTcpSocket *client, const QString &username) {
    // Thêm người chơi vào hàng đợi
    waitingQueue.enqueue(qMakePair(client, username));
    qDebug() << "Player added to queue:" << username;

    // Nếu đủ hai người chơi, tạo trận đấu
    if (waitingQueue.size() >= 2) {
        QPair<QTcpSocket *, QString> player1 = waitingQueue.dequeue();
        QPair<QTcpSocket *, QString> player2 = waitingQueue.dequeue();

        QString matchId = service->createMatch(player1.second, player2.second, player1.first, player2.first);
        if (matchId.isEmpty()) {
            // Nếu không thể tạo trận đấu, gửi lỗi cho cả hai người chơi
            QJsonObject errorResponse;
            errorResponse["type"] = "find_match_ack";
            errorResponse["status"] = "failed";
            errorResponse["message"] = "Unable to create match.";

            player1.first->write(QJsonDocument(errorResponse).toJson());
            player1.first->flush();

            player2.first->write(QJsonDocument(errorResponse).toJson());
            player2.first->flush();
            return;
        }

        // Gửi phản hồi thành công cho người chơi thứ nhất
        QJsonObject response1;
        response1["type"] = "find_match_ack";
        response1["status"] = "success";
        response1["match_id"] = matchId;
        response1["opponent"] = player2.second;
        response1["role"] = "BLACK";
        response1["message"] = "Match created successfully.";
        player1.first->write(QJsonDocument(response1).toJson());
        player1.first->flush();

        // Gửi phản hồi thành công cho người chơi thứ hai
        QJsonObject response2;
        response2["type"] = "find_match_ack";
        response2["status"] = "success";
        response2["match_id"] = matchId;
        response2["opponent"] = player1.second;
        response2["role"] = "WHITE";
        response2["message"] = "Match created successfully.";
        player2.first->write(QJsonDocument(response2).toJson());
        player2.first->flush();

        qDebug() << "Match created with ID:" << matchId
                 << "Player1:" << player1.second
                 << "Player2:" << player2.second;
    } else {
        // Nếu chưa đủ người chơi, gửi thông báo chờ
        QJsonObject waitingResponse;
        waitingResponse["type"] = "find_match_ack";
        waitingResponse["status"] = "waiting";
        waitingResponse["message"] = "Waiting for another player...";
        client->write(QJsonDocument(waitingResponse).toJson());
        client->flush();
    }
}

void MatchController::sendErrorResponse(QTcpSocket *client, const QString &errorMessage) {
    QJsonObject response;
    response["type"] = "error";
    response["message"] = errorMessage;

    QJsonDocument doc(response);
    client->write(doc.toJson());
    client->flush();

    qDebug() << "Error sent to client:" << errorMessage;
}

void MatchController::handlePlayerMove(QTcpSocket *sender, const QString &matchId, const QJsonObject &moveData) {
    QPair<QTcpSocket *, QTcpSocket *> players = service->getPlayersInMatch(matchId);

    if (players.first == nullptr || players.second == nullptr) {
        qWarning() << "Invalid match ID or players not found:" << matchId;
        sendErrorResponse(sender, "Invalid match ID or players not found.");
        return;
    }

    QTcpSocket *receiver = (sender == players.first) ? players.second : players.first;

    // QJsonObject response;
    // response["type"] = "move";
    // response["match_id"] = matchId;
    // response["move"] = moveData;

    QJsonDocument doc(moveData);
    receiver->write(doc.toJson());
    receiver->flush();

    qDebug() << "Move forwarded to opponent in match" << matchId;
}

void MatchController::removePlayerFromQueue(QTcpSocket *socket) {
    for (int i = 0; i < waitingQueue.size(); ++i) {
        if (waitingQueue[i].first == socket) {
            qDebug() << "Removing player from queue:" << waitingQueue[i].second;
            waitingQueue.removeAt(i);
            break;
        }
    }
}
