#include "MatchRepository.h"
#include <QSqlQuery>
#include <QDateTime>
#include <QDebug>
#include <QUuid>
#include <QSqlError>

MatchRepository::MatchRepository(QSqlDatabase db) : db(db) {}

QString MatchRepository::createMatch(const QString &player1, const QString &player2) {
    QString matchId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    QSqlQuery query(db);
    query.prepare("INSERT INTO matches (match_id, player1, player2, status, start_time) "
                  "VALUES (:match_id, :player1, :player2, 'ongoing', :start_time)");
    query.bindValue(":match_id", matchId);
    query.bindValue(":player1", player1);
    query.bindValue(":player2", player2);
    query.bindValue(":start_time", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        qWarning() << "Failed to insert match into database:" << query.lastError().text();
        return QString();
    }

    return matchId;
}

void MatchRepository::updateMatchStatus(const QString &matchId, const QString &status) {
    QSqlQuery query(db);
    query.prepare("UPDATE matches SET status = :status WHERE match_id = :match_id");
    query.bindValue(":status", status);
    query.bindValue(":match_id", matchId);

    if (!query.exec()) {
        qWarning() << "Failed to update match status:" << query.lastError().text();
    }
}

void MatchRepository::updateMatchResult(const QString &matchId, const QString &result, const QString &winner, const QString &loser, const QString &reason) {
    QDateTime endTime;
    QSqlQuery query(db);
    query.prepare("UPDATE match SET result = :result, winner_username = :winner, loser_username = :loser, "
                  "reason = :reason, end_time = :end_time WHERE match_id = :match_id");
    query.bindValue(":result", result);
    query.bindValue(":winner", winner);
    query.bindValue(":loser", loser);
    query.bindValue(":reason", reason);
    query.bindValue(":end_time", endTime.toString(Qt::ISODate));
    query.bindValue(":match_id", matchId);
    query.exec();

    if (query.lastError().isValid()) {
        qDebug() << "Failed to update match result:" << query.lastError();
    } else {
        qDebug() << "Match result updated successfully for match ID:" << matchId;
    }
}
