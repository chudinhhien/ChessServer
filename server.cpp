// server.cpp
#include "server.h"
#include "user.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>

ChessServer::ChessServer(QObject *parent) : QTcpServer(parent), matchmaker(new Matchmaker(this)), clientManager(new ClientManager(this)) {
    connect(this, &QTcpServer::newConnection, this, &ChessServer::onNewConnection);
    matchmaker->start();
}

bool ChessServer::startServer(int port) {
    if (!this->listen(QHostAddress::Any, port)) {
        qDebug() << "Server could not start!";
        return false;
    } else {
        qDebug() << "Server started on port:" << port;
        return true;
    }
}

void ChessServer::onNewConnection() {
    QTcpSocket *clientSocket = nextPendingConnection();
    if (!clientSocket) {
        qDebug() << "Error: No pending connection!";
        return;
    }

    connect(clientSocket, &QTcpSocket::readyRead, this, &ChessServer::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &ChessServer::onClientDisconnected);

    qDebug() << "New client connected from:" << clientSocket->peerAddress().toString();
}

void ChessServer::onClientDisconnected() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    clients.remove(clientSocket);
    clientSocket->deleteLater();
    qDebug() << "Client disconnected";
}

void ChessServer::onReadyRead() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    QByteArray data = clientSocket->readAll();

    QJsonParseError parseError;
    QJsonDocument jsonData = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON parse error:" << parseError.errorString();
        sendErrorResponse(clientSocket, "Invalid JSON format.");
        return;
    }

    if (!jsonData.isObject()) {
        qDebug() << "Received data is not a JSON object.";
        sendErrorResponse(clientSocket, "Invalid JSON object.");
        return;
    }

    QJsonObject jsonObj = jsonData.object();
    QString type = jsonObj.value("type").toString();

    if (type == "register") {
        handleRegister(jsonObj, clientSocket);
    }
    else if (type == "login") {
        handleLogin(jsonObj, clientSocket);
    }
    else if(type == "find_match"){
        matchmaker->addPlayer(clientSocket,jsonObj.value("username").toString());
    }
    else if(type == "connect") {
        QJsonObject json;
        json["type"] = "connect_ack";
        json["status"] = "success";
        json["message"] = "Login success!";
        QJsonDocument doc(json);
        QByteArray responseData = doc.toJson(QJsonDocument::Compact);
        clientSocket->write(responseData);
        clientSocket->flush();
        qDebug() << "Sent response to client:" << responseData;
    }
    else if(type == "update_profile") {
        clientManager->handleUpdateProfileAck(clientSocket,jsonObj);
    }
    else if(type == "change_password") {
        clientManager->handleChangePasswordAck(clientSocket,jsonObj);
    }
    else {
        qDebug() << "Unknown request type:" << type;
        sendErrorResponse(clientSocket, "Unknown request type.");
    }
}

void ChessServer::sendResponse(QTcpSocket *clientSocket, const QString &type, const QString &status, const QString &message, const QString &token) {
    QJsonObject json;
    json["type"] = type;
    json["status"] = status;
    json["message"] = message;
    if (!token.isEmpty()) {
        json["token"] = token;
    }
    QJsonDocument doc(json);
    QByteArray responseData = doc.toJson(QJsonDocument::Compact);

    if (clientSocket && clientSocket->isWritable()) {
        clientSocket->write(responseData);
        clientSocket->flush();
        qDebug() << "Sent response to client:" << responseData;
    } else {
        qDebug() << "Client socket is not writable!";
    }
}

void ChessServer::sendOnlinePlayers(QTcpSocket *clientSocket) {
    QJsonObject playerListMessage;
    playerListMessage["type"] = "list_players";
    playerListMessage["status"] = "success";

    // Tạo danh sách các người chơi online
    QJsonArray playerArray;
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        QJsonObject player;
        player["username"] = it.value();  // Lấy username từ connectedClients
        playerArray.append(player);
    }

    playerListMessage["players"] = playerArray;

    QJsonDocument doc(playerListMessage);
    QByteArray data = doc.toJson();

    clientSocket->write(data);
    clientSocket->flush();
    qDebug() << "Sent player list to client.";
}

void ChessServer::sendErrorResponse(QTcpSocket *clientSocket, const QString &errorMessage) {
    sendResponse(clientSocket, "error", "failed", errorMessage);
}

void ChessServer::handleRegister(const QJsonObject &jsonObj, QTcpSocket *clientSocket) {
    User user(jsonObj);
    QString errorMessage;
    bool success = authManager.registerUser(user, errorMessage);

    if (success) {
        sendResponse(clientSocket, "register_ack", "success", "Registration successful!");
    } else {
        sendResponse(clientSocket, "register_ack", "failed", errorMessage);
    }
}

void ChessServer::handleLogin(const QJsonObject &jsonObj, QTcpSocket *clientSocket) {
    QString username = jsonObj.value("username").toString();
    QString password = jsonObj.value("password").toString();

    QString token;
    QString errorMessage;
    bool success = authManager.loginUser(clientSocket,username, password, token, errorMessage);

    if (success) {
        clients[clientSocket] = username;
        // sendResponse(clientSocket, "login_ack", "success", "Login successful!", token);
        // sendOnlinePlayers(clientSocket);
    } else {
        sendResponse(clientSocket, "login_ack", "failed", errorMessage);
    }
}
