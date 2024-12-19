#include "Game.h"
#include <QDebug>

Game::Game(QTcpSocket *player1, QTcpSocket *player2, QString matchID, QObject *parent)
    : QThread(parent), player1(player1), player2(player2), matchID(matchID) {}

Game::~Game() {
    // Dọn dẹp tài nguyên nếu cần
}

void Game::run() {
    processGame(player1, player2);
}

void Game::processGame(QTcpSocket *player1, QTcpSocket *player2) {
    qDebug() << "Game started between Player1 and Player2.";

    // Gửi tín hiệu thay vì ghi trực tiếp vào socket
    emit sendMessage(player1, "Your turn\n");
    emit sendMessage(player2, "Opponent's turn\n");

    // Thông báo kết thúc trận đấu
    emit sendMessage(player1, "Game Over. Player1 wins!\n");
    emit sendMessage(player2, "Game Over. Player2 loses!\n");
}
