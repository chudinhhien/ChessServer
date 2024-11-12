#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ChessServer server;
    server.startServer(5500);

    return a.exec();
}
