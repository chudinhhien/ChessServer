#include "AuthService.h"

AuthService::AuthService(UserRepository *repository) : repository(repository) {}

bool AuthService::registerUser(const QString &name, const QString &username, const QString &password, QString &errorMessage) {
    if (username.isEmpty() || password.isEmpty()) {
        errorMessage = "Username and password cannot be empty.";
        return false;
    }

    if (repository->userExists(username)) {
        errorMessage = "User already exists.";
        return false;
    }

    if (!repository->createUser(name, username, password)) {
        errorMessage = "Failed to create user.";
        return false;
    }

    return true;
}

bool AuthService::loginUser(const QString &username, const QString &password, QString &errorMessage) {
    if (username.isEmpty() || password.isEmpty()) {
        errorMessage = "Username and password cannot be empty.";
        return false;
    }

    if (!repository->validateUser(username, password)) {
        errorMessage = "Invalid username or password.";
        return false;
    }    
    return true;
}

void AuthService::playerLoggedIn(QTcpSocket *socket, const QString &username) {
    repository->addOnlinePlayer(socket, username);
}

void AuthService::playerLoggedOut(QTcpSocket *socket) {
    repository->removeOnlinePlayer(socket);
}

QString AuthService::getUsernameBySocket(QTcpSocket *socket) const {
    return repository->getUsernameBySocket(socket);
}


QList<User> AuthService::getOnlinePlayers() const {
    return repository->getOnlinePlayers();
}

void AuthService::updatePlayerStatus(QTcpSocket *socket, const QString &status) {
    repository->updatePlayerStatus(socket, status);
    qDebug() << "Player status updated to:" << status;
}

User AuthService::getUserInfo(const QString &username) const {
    return repository->getUserByUsername(username);
}

QTcpSocket* AuthService::getSocketByUserName(const QString &username) const {
    return repository->getSocketByUserName(username);
};

void AuthService::updatePlayerElo(const QString &username, int newElo) {
    repository->updatePlayerElo(username, newElo);
}

