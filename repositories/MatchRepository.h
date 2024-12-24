#ifndef MATCHREPOSITORY_H
#define MATCHREPOSITORY_H

#include <QString>
#include <QSqlDatabase>

class MatchRepository {
public:
    MatchRepository(QSqlDatabase db);

    QString createMatch(const QString &player1, const QString &player2);
    void updateMatchStatus(const QString &matchId, const QString &status);

private:
    QSqlDatabase db;
};

#endif // MATCHREPOSITORY_H
