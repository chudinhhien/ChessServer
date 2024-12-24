// server.h
#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include "authenticationmanager.h"
#include "Matchmaker.h"
#include "ClientManager.h"
#include "Game.h"

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
    Matchmaker *matchmaker;
    ClientManager *clientManager;
    QMap<QTcpSocket*, QString> clients;
    AuthenticationManager authManager;
    QHash<QTcpSocket *, Game *> activeGames;  // Ánh xạ giữa client socket và Game

    void handleGameRequest(QTcpSocket *clientSocket, const QJsonObject &jsonObj);
    void sendResponse(QTcpSocket *clientSocket, const QString &type, const QString &status, const QString &message, const QString &token = "");
    void sendErrorResponse(QTcpSocket *clientSocket, const QString &errorMessage);
    void sendOnlinePlayers(QTcpSocket *clientSocket);

    void handleRegister(const QJsonObject &jsonObj, QTcpSocket *clientSocket);
    void handleLogin(const QJsonObject &jsonObj, QTcpSocket *clientSocket);
};

#endif // SERVER_H
