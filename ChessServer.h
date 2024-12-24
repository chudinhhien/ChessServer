#ifndef CHESSSERVER_H
#define CHESSSERVER_H

#include <QTcpServer>
#include "MatchController.h"
#include "MatchService.h"
#include "MatchRepository.h"

#include "AuthController.h"
#include "AuthService.h"
#include "UserRepository.h"

#include <RoomController.h>
#include <RoomService.h>
#include <RoomRepository.h>

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

    AuthController *authController;
    AuthService *authService;
    UserRepository *userRepository;

    RoomController *roomController;
    RoomService *roomService;
    RoomRepository *roomRepository;

};

#endif // CHESSSERVER_H
