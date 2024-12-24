#ifndef MATCHSERVICE_H
#define MATCHSERVICE_H

#include <QString>
#include "MatchRepository.h"

class MatchService {
public:
    MatchService(MatchRepository *repository);

    QString createMatch(const QString &player1, const QString &player2);

private:
    MatchRepository *repository;
};

#endif // MATCHSERVICE_H
