#include "AuthController.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

AuthController::AuthController(AuthService *service, QObject *parent)
    : QObject(parent), service(service) {}

void AuthController::handleRegister(QTcpSocket *client, const QString &name ,const QString &username, const QString &password) {
    QString errorMessage;
    if (service->registerUser(name, username, password, errorMessage)) {
        sendResponse(client, "register_ack", "success", "Registration successful!");
    } else {
        sendResponse(client, "register_ack", "failed", errorMessage);
    }
}

void AuthController::handleLogin(QTcpSocket *client, const QString &username, const QString &password) {
    QString errorMessage;
    if (service->loginUser(username, password, errorMessage)) {
        service->playerLoggedIn(client, username);

        // Lấy thông tin người dùng
        User user = service->getUserInfo(username);

        // Gửi phản hồi bao gồm thông tin username và elo
        QJsonObject response;
        response["type"] = "login_ack";
        response["status"] = "success";
        response["message"] = "Login successful!";
        response["name"] = user.getName();
        response["elo"] = user.getElo();

        QJsonDocument doc(response);
        client->write(doc.toJson());
        client->flush();
    } else {
        sendResponse(client, "login_ack", "failure", errorMessage);
    }
}

void AuthController::sendResponse(QTcpSocket *client, const QString &type, const QString &status, const QString &message) {
    QJsonObject json;
    json["type"] = type;
    json["status"] = status;
    json["message"] = message;

    QJsonDocument doc(json);
    client->write(doc.toJson());
    client->flush();
}

void AuthController::handleGetOnlinePlayers(QTcpSocket *client) {
    QList<User> onlinePlayers = service->getOnlinePlayers();

    QJsonArray playersArray;
    QString requestingUsername = service->getUsernameBySocket(client); // Lấy username từ socket

    for (const User &user : onlinePlayers) {
        if (user.getUsername() == requestingUsername) {
            continue; // Bỏ qua chính người dùng đang gửi yêu cầu
        }

        QJsonObject playerJson;
        playerJson["name"] = user.getName();
        playerJson["username"] = user.getUsername();
        playerJson["elo"] = user.getElo();
        playerJson["state"] = user.getState();
        playersArray.append(playerJson);
    }

    QJsonObject response;
    response["type"] = "get_list_player_ack";
    response["status"] = "success";
    response["message"] = "List of online players retrieved successfully.";
    response["players"] = playersArray;

    QJsonDocument doc(response);
    client->write(doc.toJson());
    client->flush();

    qDebug() << "Sent list of online players to client. Excluded username:" << requestingUsername;
}
