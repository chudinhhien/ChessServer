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
