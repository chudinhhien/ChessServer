#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include <QString>
#include <QSqlDatabase>
#include <QTcpSocket>
#include <User.h>

class UserRepository {
public:
    explicit UserRepository(QSqlDatabase db);

    bool createUser(const QString &name, const QString &username, const QString &password);
    bool validateUser(const QString &username, const QString &password);
    bool userExists(const QString &username);

    void addOnlinePlayer(QTcpSocket *socket, const QString &username);
    void removeOnlinePlayer(QTcpSocket *socket);
    void updatePlayerStatus(QTcpSocket *socket, const QString &status);
    QString getUsernameBySocket(QTcpSocket *socket) const;
    User getUserByUsername(const QString &username) const;
    QTcpSocket *getSocketByUserName(const QString &username) const;
    QList<User> getOnlinePlayers() const;
    void updatePlayerElo(const QString &username, int newElo);

private:
    QMap<QTcpSocket *, User> onlinePlayers;
    QSqlDatabase db;
};

#endif // USERREPOSITORY_H
