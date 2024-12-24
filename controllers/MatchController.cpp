#include "MatchController.h"
#include <QJsonDocument>
#include <QJsonObject>

MatchController::MatchController(MatchService *service, QObject *parent) : QObject(parent), service(service) {}

void MatchController::handleFindMatch(QTcpSocket *client, const QString &username) {
    // Tìm hoặc tạo một trận đấu
    QString matchId = service->createMatch(username, "opponent_username");  // Ví dụ
    if (matchId.isEmpty()) {
        QJsonObject response;
        response["type"] = "find_match_ack";
        response["status"] = "failed";
        response["message"] = "Unable to find or create match.";
        client->write(QJsonDocument(response).toJson());
        return;
    }

    QJsonObject response;
    response["type"] = "find_match_ack";
    response["status"] = "success";
    response["match_id"] = matchId;
    response["message"] = "Match created successfully.";
    client->write(QJsonDocument(response).toJson());
}
