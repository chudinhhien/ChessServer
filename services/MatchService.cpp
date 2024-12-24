#include "MatchService.h"

MatchService::MatchService(MatchRepository *repository) : repository(repository) {}

QString MatchService::createMatch(const QString &player1, const QString &player2, QTcpSocket *socket1, QTcpSocket *socket2) {
    // Tạo matchId và lưu vào cơ sở dữ liệu
    QString matchId = repository->createMatch(player1, player2);
    if (!matchId.isEmpty()) {
        // Lưu socket vào bộ nhớ tạm (activeMatches)
        activeMatches[matchId] = qMakePair(socket1, socket2);
    }
    return matchId;
}

QPair<QTcpSocket *, QTcpSocket *> MatchService::getPlayersInMatch(const QString &matchId) {
    if (activeMatches.contains(matchId)) {
        return activeMatches[matchId];
    }
    return QPair<QTcpSocket *, QTcpSocket *>(nullptr, nullptr);
}
