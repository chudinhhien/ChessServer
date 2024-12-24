#ifndef ROOMSERVICE_H
#define ROOMSERVICE_H

#include "RoomRepository.h"
#include "AuthService.h"
#include "MatchService.h"
#include <QString>
#include <QTcpSocket>

class RoomService {
public:
    explicit RoomService(RoomRepository *repository, AuthService *authService, MatchService *matchService);

    bool createRoom(const QString &username, QString &roomId, QString &message);
    void sendInvite(const QString &fromPlayer, const QString &toPlayer, QTcpSocket *fromSocket, QTcpSocket *toSocket, const QString &fromPlayerName);
    void handleInviteResponse(const QString &toPlayer, bool accepted);

private:
    RoomRepository *repository;
    AuthService *authService;
    MatchService *matchService; // Thêm MatchService
    QMap<QString, QString> pendingInvites;
};

#endif // ROOMSERVICE_H
