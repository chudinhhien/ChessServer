#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QTcpSocket>

class ClientManager : public QObject
{
    Q_OBJECT

public:
    explicit ClientManager(QObject *parent = nullptr);
    ~ClientManager();
    void handleUpdateProfileAck(QTcpSocket *socket, const QJsonObject &jsonObj);
    void handleChangePasswordAck(QTcpSocket *socket, const QJsonObject &jsonObj);
    void handleCreateRoomAck(QTcpSocket *socket, const QJsonObject &jsonObj);

signals:
    void sendMessageToSocket(QTcpSocket *socket, QByteArray data);
};

#endif // CLIENTMANAGER_H
