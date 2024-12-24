#ifndef ROOMCONTROLLER_H
#define ROOMCONTROLLER_H

#include "RoomService.h"
#include "AuthService.h"
#include <QTcpSocket>
#include <QJsonObject>
#include <QObject>

class RoomController : public QObject {
    Q_OBJECT
public:
    explicit RoomController(RoomService *service, AuthService *authService, QObject *parent = nullptr);

    void handleCreateRoom(QTcpSocket *client, const QString &username);
    void handleInvite(QTcpSocket *client, const QString &fromPlayer, const QString &toPlayer, const QString &fromPlayerName);
    void handleInviteResponse(QTcpSocket *client, const QString &toPlayer, bool accepted);
    QString createMatch(const QString &player1, const QString &player2);
private:
    RoomService *service;
    AuthService *authService; // Thêm UserService để sử dụng
};

#endif // ROOMCONTROLLER_H
