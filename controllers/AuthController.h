#ifndef AUTHCONTROLLER_H
#define AUTHCONTROLLER_H

#include <QObject>
#include <QTcpSocket>
#include "services/AuthService.h"

class AuthController : public QObject {
    Q_OBJECT

public:
    explicit AuthController(AuthService *service, QObject *parent = nullptr);

    void handleRegister(QTcpSocket *client, const QString &name,const QString &username, const QString &password);
    void handleLogin(QTcpSocket *client, const QString &username, const QString &password);
    void handleGetOnlinePlayers(QTcpSocket *client);
private:
    AuthService *service;

    void sendResponse(QTcpSocket *client, const QString &type, const QString &status, const QString &message);
};

#endif // AUTHCONTROLLER_H
