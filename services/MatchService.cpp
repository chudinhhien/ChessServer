#include "MatchService.h"
#include <QJsonObject>
#include <QJsonDocument>

MatchService::MatchService(MatchRepository *repository, AuthService *authService)
    : repository(repository), authService(authService) {}

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

void MatchService::calculateAndUpdateElo(const QString &matchId, const QString &result, const QString &winner, const QString &loser) {
    if (!activeMatches.contains(matchId)) {
        qDebug() << "Match ID not found in active matches.";
        return;
    }

    if (winner.isEmpty() || loser.isEmpty()) {
        qDebug() << "Winner or loser is empty for match ID:" << matchId;
        return;
    }

    // Lấy Elo hiện tại của hai người chơi
    int winnerElo = authService->getUserInfo(winner).getElo();
    int loserElo = authService->getUserInfo(loser).getElo();

    // Xác định điểm thực tế (S)
    double S1 = 0.0, S2 = 0.0;
    if (result == "Win") {
        S1 = 1.0; // Người thắng
        S2 = 0.0; // Người thua
    } else if (result == "Draw") {
        S1 = 0.5; // Cả hai hòa
        S2 = 0.5;
    }

    // Tính điểm dự đoán
    double E1 = 1.0 / (1.0 + pow(10.0, (loserElo - winnerElo) / 400.0));
    double E2 = 1.0 / (1.0 + pow(10.0, (winnerElo - loserElo) / 400.0));

    // Hệ số K
    const int K = 32;

    // Tính điểm Elo mới
    int newWinnerElo = static_cast<int>(winnerElo + K * (S1 - E1));
    int newLoserElo = static_cast<int>(loserElo + K * (S2 - E2));

    // Cập nhật Elo cho cả hai người chơi
    authService->updatePlayerElo(winner, newWinnerElo);
    authService->updatePlayerElo(loser, newLoserElo);

    qDebug() << "Updated Elo: Winner (" << winner << ") -> " << newWinnerElo
             << ", Loser (" << loser << ") -> " << newLoserElo;
}

void MatchService::updateMatchResult(const QString &matchId, const QString &result, const QString &loser) {
    QString winner;
    QString reason;

    // Xác định người thắng
    if (activeMatches[matchId].first == authService->getSocketByUserName(loser)) {
        winner = authService->getUsernameBySocket(activeMatches[matchId].second);
    } else {
        winner = authService->getUsernameBySocket(activeMatches[matchId].first);
    }

    // Cập nhật kết quả trận đấu
    repository->updateMatchResult(matchId, result, winner, loser, reason);

    // Tính và cập nhật Elo
    calculateAndUpdateElo(matchId, result, winner, loser);

    // Gửi thông điệp cho người thắng
    QTcpSocket *winnerSocket = authService->getSocketByUserName(winner);
    if (winnerSocket) {
        QJsonObject winnerMessage;
        winnerMessage["type"] = "winner";
        winnerMessage["message"] = "Congratulations! You are the winner!";
        winnerMessage["elo"] = authService->getUserInfo(winner).getElo(); // Elo mới sau khi cập nhật

        QJsonDocument doc(winnerMessage);
        winnerSocket->write(doc.toJson());
        winnerSocket->flush();
        qDebug() << "Winner notification sent to:" << winner;
    } else {
        qDebug() << "Winner socket not found for:" << winner;
    }
}
