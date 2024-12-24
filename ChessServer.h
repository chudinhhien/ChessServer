#ifndef CHESSSERVER_H
#define CHESSSERVER_H

#include <QTcpServer>
#include "controllers/MatchController.h"
#include "services/MatchService.h"
#include "repositories/MatchRepository.h"

class ChessServer : public QTcpServer {
    Q_OBJECT

public:
    explicit ChessServer(QObject *parent = nullptr);
    bool startServer(int port);

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onReadyRead();

private:
    MatchController *matchController;
    MatchService *matchService;
    MatchRepository *matchRepository;
};

#endif // CHESSSERVER_H
