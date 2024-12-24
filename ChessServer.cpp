#include "ChessServer.h"
#include <QSqlDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QSqlError>
#include <utils.h>

ChessServer::ChessServer(QObject *parent) : QTcpServer(parent) {
    // Thiết lập cơ sở dữ liệu
    QString host = utils::getEnvVariableFromFile("config.env", "host");
    QString dbName = utils::getEnvVariableFromFile("config.env", "dbName");
    QString user = utils::getEnvVariableFromFile("config.env", "username");
    QString password = utils::getEnvVariableFromFile("config.env", "password");

    QString connectionName = "chess_connection";  // Tên kết nối cụ thể
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        if (db.isOpen()) {
            qDebug() << "Database connection already open.";
        } else {
            qCritical() << "Database connection exists but not open.";
            return;
        }
    } else {
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setHostName(host);
        db.setDatabaseName(dbName);
        db.setUserName(user);
        db.setPassword(password);

        if (!db.open()) {
            qCritical() << "Failed to connect to database:" << db.lastError().text();
            return;
        }
        qDebug() << "Database connected successfully.";
    }
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    // Khởi tạo các tầng Repository, Service, Controller
    matchRepository = new MatchRepository(db);
    matchService = new MatchService(matchRepository);
    matchController = new MatchController(matchService, this);

    userRepository = new UserRepository(db);
    authService = new AuthService(userRepository);
    authController = new AuthController(authService, this);

    roomRepository = new RoomRepository(db);
    roomService = new RoomService(roomRepository, authService, matchService);
    roomController = new RoomController(roomService, authService, this);

    connect(this, &QTcpServer::newConnection, this, &ChessServer::onNewConnection);
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
    if (clientSocket) {
        matchController->removePlayerFromQueue(clientSocket);
        authService->playerLoggedOut(clientSocket);
        clientSocket->deleteLater();
        qDebug() << "Client disconnected";
    }
}

void ChessServer::onReadyRead() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    QByteArray data = clientSocket->readAll();

    QJsonDocument jsonData = QJsonDocument::fromJson(data);
    if (!jsonData.isObject()) {
        qDebug() << "Invalid JSON received from client.";
        return;
    }

    QJsonObject jsonObj = jsonData.object();
    QString type = jsonObj.value("type").toString();
    if (type == "register") {
        QString name = jsonObj.value("name").toString();
        QString username = jsonObj.value("username").toString();
        QString password = jsonObj.value("password").toString();
        authController->handleRegister(clientSocket,name, username, password);
    } else if (type == "login") {
        QString username = jsonObj.value("username").toString();
        QString password = jsonObj.value("password").toString();
        authController->handleLogin(clientSocket, username, password);
    } else if (type == "find_match") {
        QString username = jsonObj.value("username").toString();
        matchController->handleFindMatch(clientSocket, username);
    } else if (type == "connect") {
        QJsonObject json;
        json["type"] = "connect_ack";
        json["status"] = "success";
        json["message"] = "Login success!";
        QJsonDocument doc(json);
        QByteArray responseData = doc.toJson(QJsonDocument::Compact);
        clientSocket->write(responseData);
        clientSocket->flush();
        qDebug() << "Sent response to client:" << responseData;
    } else if (type == "move") {
        QString matchId = jsonObj.value("game_id").toString();
        matchController->handlePlayerMove(clientSocket, matchId, jsonObj);
    } else if (type == "create_room") {
        QString username = jsonObj.value("username").toString();
        roomController->handleCreateRoom(clientSocket, username);
    } else if (type == "get_list_player") {
        authController->handleGetOnlinePlayers(clientSocket);
    } else if( type == "invite_player") {
        QString username = jsonObj.value("username").toString();
        QString name = jsonObj.value("name").toString();
        QString invite_player = jsonObj.value("invite_player").toString();
        roomController->handleInvite(clientSocket,username, invite_player, name);
    }
    else {
        qDebug() << "Unknown request type:" << type;
    }
}
