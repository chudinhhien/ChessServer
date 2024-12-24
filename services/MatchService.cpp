#include "MatchService.h"

MatchService::MatchService(MatchRepository *repository) : repository(repository) {}

QString MatchService::createMatch(const QString &player1, const QString &player2) {
    return repository->createMatch(player1, player2);
}
