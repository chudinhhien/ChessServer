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
    QString moveData = QString::fromUtf8(clientSocket->readAll());

    qDebug() << "Move received:" << moveData;

    // Gửi nước đi tới tất cả các client khác
    for (QTcpSocket *client : clients) {
        if (client != clientSocket) {
            client->write(moveData.toUtf8());
        }
    }
}
