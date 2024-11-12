#include "server.h"
#include <QDebug>

ChessServer::ChessServer(QObject *parent) : QTcpServer(parent)
{
    connect(this, &QTcpServer::newConnection, this, &ChessServer::onNewConnection);
}

void ChessServer::startServer(int port)
{
    if (!this->listen(QHostAddress::Any, port)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started on port:" << port;
    }
}

void ChessServer::onNewConnection()
{
    QTcpSocket *clientSocket = nextPendingConnection();
    clients.append(clientSocket);

    connect(clientSocket, &QTcpSocket::readyRead, this, &ChessServer::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &ChessServer::onClientDisconnected);

    qDebug() << "New client connected from:" << clientSocket->peerAddress().toString();
}

void ChessServer::onClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    clients.removeAll(clientSocket);
    clientSocket->deleteLater();
    qDebug() << "Client disconnected";
}

void ChessServer::onReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    QByteArray data = clientSocket->readAll();//Đọc data từ socket

    //Convert sang json
    QJsonDocument jsonData = QJsonDocument::fromJson(data);

    if(jsonData.isObject()) {
        QJsonObject jsonObj = jsonData.object();
        QString type = jsonObj.value("type").toString();
        if(type == "register") {
            QJsonObject json;
            json["type"] = "register_ack";
            json["status"] = "success";
            json["message"] = "Registration successful!";
            QJsonDocument doc(json);
            QByteArray data = doc.toJson(QJsonDocument::Compact);
            if (clientSocket && clientSocket->isWritable()) {
                clientSocket->write(data);
                clientSocket->flush(); // Đảm bảo dữ liệu được gửi ngay lập tức
                qDebug() << "Sent response to client:" << data;
            } else {
                qDebug() << "Client socket is not writable!";
            }
        }
    } else {
        qDebug() << "Received data is not a JSON object.";
    }
}
