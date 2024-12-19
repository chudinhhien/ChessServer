#ifndef MATCHMAKER_H
#define MATCHMAKER_H

#include <QThread>
#include <QTcpSocket>
#include <QMutex>
#include <QQueue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <databasemanager.h>

class Matchmaker : public QThread {
    Q_OBJECT

public:
    explicit Matchmaker(QObject *parent = nullptr);
    ~Matchmaker() override;

    void addPlayer(QTcpSocket *playerSocket, QString username);

protected:
    void run() override;
signals:
    void sendMessageToSocket(QTcpSocket *socket, QByteArray data);
private:
    void findMatch();
    void startMatch(QTcpSocket *player1, QTcpSocket *player2);
    void sendMatchStart(QTcpSocket *player1, QTcpSocket *player2, const QString &matchId);
    void sendErrorResponse(QTcpSocket *clientSocket, const QString &errorMessage);
    QString createMatchInDB(const QString &player1, const QString &player2);

    QQueue<QTcpSocket *> waitingPlayers;
    QMutex mutex;
    QMap<QTcpSocket *, QString> playerUsernames;
};

#endif // MATCHMAKER_H
