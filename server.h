#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QList>

class ChessServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit ChessServer(QObject *parent = nullptr);
    void startServer(int port);

signals:
    void newMoveReceived(const QString &moveData);

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onReadyRead();

private:
    QList<QTcpSocket *> clients;  // Danh sách các client kết nối
};

#endif // SERVER_H
