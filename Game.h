#ifndef GAME_H
#define GAME_H

#include <QThread>
#include <QTcpSocket>

class Game : public QThread {
    Q_OBJECT

public:
    Game(QTcpSocket *player1, QTcpSocket *player2, QString matchID, QObject *parent = nullptr);
    ~Game() override;
signals:
    void sendMessage(QTcpSocket *socket, const QByteArray &data);
protected:
    void run() override;

private:
    void processGame(QTcpSocket *player1, QTcpSocket *player2);

    QTcpSocket *player1;
    QTcpSocket *player2;
    QString matchID;
};

#endif // GAME_H
