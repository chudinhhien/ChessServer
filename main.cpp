#include <QCoreApplication>
#include "ChessServer.h"
#include "Logger.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    qInstallMessageHandler(customMessageHandler);

    ChessServer server;
    if (!server.startServer(5500)) {
        qCritical() << "Failed to start server.";
        return -1;
    }

    qDebug() << "Chess server is running.";
    return app.exec();
}
