#include "User.h"

// Constructor mặc định
User::User() : elo(0) {}

// Constructor với tham số
User::User(const QString &name, const QString &username, int elo, const QString &state)
    : name(name), username(username), elo(elo), state(state) {}

// Getters
QString User::getName() const {
    return name;
}

QString User::getUsername() const {
    return username;
}

int User::getElo() const {
    return elo;
}

QString User::getState() const {
    return state;
}

// Setters
void User::setName(const QString &name) {
    this->name = name;
}

void User::setUsername(const QString &username) {
    this->username = username;
}

void User::setElo(int elo) {
    this->elo = elo;
}

void User::setState(const QString &state) {
    this->state = state;
}
