#ifndef MATCHCONTROLLER_H
#define MATCHCONTROLLER_H

#include <QObject>
#include <QQueue>
#include <QTcpSocket>
#include "MatchService.h"

class MatchController : public QObject {
    Q_OBJECT

public:
    explicit MatchController(MatchService *service, QObject *parent = nullptr);

    void handleFindMatch(QTcpSocket *client, const QString &username);

    void handlePlayerMove(QTcpSocket *sender, const QString &matchId, const QJsonObject &moveData);

private:
    MatchService *service;
    QQueue<QPair<QTcpSocket *, QString>> waitingQueue;
    void sendErrorResponse(QTcpSocket *client, const QString &errorMessage);
};

#endif // MATCHCONTROLLER_H

