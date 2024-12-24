#ifndef MATCHCONTROLLER_H
#define MATCHCONTROLLER_H

#include <QObject>
#include <QTcpSocket>
#include "MatchService.h"

class MatchController : public QObject {
    Q_OBJECT

public:
    explicit MatchController(MatchService *service, QObject *parent = nullptr);

    void handleFindMatch(QTcpSocket *client, const QString &username);

private:
    MatchService *service;
};

#endif // MATCHCONTROLLER_H

