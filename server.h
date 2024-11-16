// server.h
#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include "authenticationmanager.h"

class ChessServer : public QTcpServer {
    Q_OBJECT
public:
    explicit ChessServer(QObject *parent = nullptr);
    bool startServer(int port);
    void onNewConnection();
private slots:
    void onReadyRead();
    void onClientDisconnected();

private:
    QList<QTcpSocket*> clients;
    AuthenticationManager authManager;

    void sendResponse(QTcpSocket *clientSocket, const QString &type, const QString &status, const QString &message, const QString &token = "");
    void sendErrorResponse(QTcpSocket *clientSocket, const QString &errorMessage);

    void handleRegister(const QJsonObject &jsonObj, QTcpSocket *clientSocket);
    void handleLogin(const QJsonObject &jsonObj, QTcpSocket *clientSocket);
};

#endif // SERVER_H
