#ifndef MATCHSERVICE_H
#define MATCHSERVICE_H

#include <QString>
#include <QMap>
#include <QTcpSocket>
#include "AuthService.h"
#include "MatchRepository.h"

class MatchService {
public:
    explicit MatchService(MatchRepository *repository, AuthService *authService);

    // Tạo một trận đấu và lưu thông tin socket của hai người chơi
    QString createMatch(const QString &player1, const QString &player2, QTcpSocket *socket1, QTcpSocket *socket2);

    // Lấy danh sách socket của hai người chơi trong một trận đấu
    QPair<QTcpSocket *, QTcpSocket *> getPlayersInMatch(const QString &matchId);

    void updateMatchResult(const QString &matchId, const QString &result, const QString &loser);
    void calculateAndUpdateElo(const QString &matchId, const QString &result);
    void calculateAndUpdateElo(const QString &matchId, const QString &result, const QString &winner, const QString &loser);
private:
    MatchRepository *repository;
    AuthService *authService;
    QMap<QString, QPair<QTcpSocket *, QTcpSocket *>> activeMatches; // Lưu matchId và cặp socket của hai người chơi
};

#endif // MATCHSERVICE_H
