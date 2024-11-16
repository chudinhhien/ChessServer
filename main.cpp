#include <QCoreApplication>
#include "databasemanager.h"
#include "authenticationmanager.h"
#include "server.h"
#include "utils.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Khởi tạo DatabaseManager (Singleton)
    DatabaseManager& dbManager = DatabaseManager::instance();

    // Mở kết nối cơ sở dữ liệu
    if (!dbManager.openConnection()) {
        qCritical() << "Failed to connect to the database. Exiting application.";
        return -1;
    }

    // Tạo bảng users nếu chưa tồn tại
    dbManager.createTableForUser();

    // Khởi tạo AuthenticationManager
    AuthenticationManager authManager;

    // Khởi tạo và khởi động ChessServer
    ChessServer server;
    quint16 port = utils::getEnvVariableFromFile("config.env","PORT").toShort(); // Chọn cổng bạn muốn server lắng nghe
    if (!server.startServer(port)) {
        qCritical() << "Failed to start the ChessServer. Exiting application.";
        return -1;
    }

    // Bắt đầu vòng lặp sự kiện của Qt
    return a.exec();
}
