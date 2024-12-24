#ifndef GAME_H
#define GAME_H

#include <QThread>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QDebug>

class Game : public QThread {
    Q_OBJECT

public:
    Game(QTcpSocket *player1Socket, QTcpSocket *player2Socket, QString matchID, QObject *parent = nullptr);
    ~Game() override;
    void handlePlayerMove(QTcpSocket *sender, QTcpSocket *receiver);
    QTcpSocket *player1;
    QTcpSocket *player2;
    QString matchID;

signals:
    void sendMessage(QTcpSocket *socket, const QByteArray &data);

protected:
    void run() override;

private:
    void forwardMove(QTcpSocket *receiver, const QByteArray &moveData);
    void sendError(QTcpSocket *socket, const QString &errorMessage);
};

#endif // GAME_H
