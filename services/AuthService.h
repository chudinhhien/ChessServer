#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "repositories/UserRepository.h"

class AuthService {
public:
    explicit AuthService(UserRepository *repository);

    bool registerUser(const QString &name, const QString &username, const QString &password, QString &errorMessage);
    bool loginUser(const QString &username, const QString &password, QString &errorMessage);

    void playerLoggedIn(QTcpSocket *socket, const QString &username);
    void playerLoggedOut(QTcpSocket *socket);
    void updatePlayerStatus(QTcpSocket *socket, const QString &status);
    QList<User> getOnlinePlayers() const;
private:
    UserRepository *repository;
};

#endif // AUTHSERVICE_H
