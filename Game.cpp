#include "Game.h"

Game::Game(QTcpSocket *player1Socket, QTcpSocket *player2Socket, QString matchID, QObject *parent)
    : QThread(parent), player1(player1Socket), player2(player2Socket), matchID(std::move(matchID)) {
    // Kết nối tín hiệu sẵn sàng đọc dữ liệu từ từng socket
    connect(player1, &QTcpSocket::readyRead, this, [this]() { handlePlayerMove(player1, player2); });
    connect(player2, &QTcpSocket::readyRead, this, [this]() { handlePlayerMove(player2, player1); });
}

Game::~Game() {
    player1->deleteLater();
    player2->deleteLater();
}

void Game::run() {
    // Chạy vòng lặp sự kiện để giữ cho thread hoạt động
    exec();
}

void Game::handlePlayerMove(QTcpSocket *sender, QTcpSocket *receiver) {
    QByteArray data = sender->readAll();

    // Parse JSON từ dữ liệu nhận được
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON received from player.";
        sendError(sender, "Invalid JSON format");
        return;
    }

    QJsonObject json = doc.object();
    QString type = json.value("type").toString();

    if (type == "move") {
        forwardMove(receiver, data);  // Chuyển tiếp nước đi tới đối thủ
    } else {
        qWarning() << "Unexpected request type:" << type;
        sendError(sender, "Unexpected request type");
    }
}

void Game::forwardMove(QTcpSocket *receiver, const QByteArray &moveData) {
    if (receiver->state() == QAbstractSocket::ConnectedState) {
        emit sendMessage(receiver, moveData);
        receiver->write(moveData);
        receiver->flush();
    } else {
        qWarning() << "Receiver is not connected. Cannot forward move.";
    }
}

void Game::sendError(QTcpSocket *socket, const QString &errorMessage) {
    QJsonObject errorResponse;
    errorResponse["type"] = "error";
    errorResponse["message"] = errorMessage;

    QJsonDocument doc(errorResponse);
    QByteArray data = doc.toJson();

    socket->write(data);
    socket->flush();

    qDebug() << "Sent error message to player:" << errorMessage;
}
