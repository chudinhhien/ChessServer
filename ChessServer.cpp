#include "ChessServer.h"
#include <QSqlDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QSqlError>

ChessServer::ChessServer(QObject *parent) : QTcpServer(parent) {
    // Thiết lập cơ sở dữ liệu
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("chess_server.db");
    if (!db.open()) {
        qCritical() << "Failed to connect to database:" << db.lastError().text();
        return;
    }

    // Khởi tạo các tầng Repository, Service, Controller
    matchRepository = new MatchRepository(db);
    matchService = new MatchService(matchRepository);
    matchController = new MatchController(matchService, this);
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

    if (type == "find_match") {
        QString username = jsonObj.value("username").toString();
        matchController->handleFindMatch(clientSocket, username);
    } else {
        qDebug() << "Unknown request type:" << type;
    }
}
