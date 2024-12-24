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

private:
    RoomService *service;
    AuthService *authService; // Thêm UserService để sử dụng
};

#endif // ROOMCONTROLLER_H
