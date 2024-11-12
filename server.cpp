#include "server.h"
#include <QDebug>
#include <QFile> // Thêm dòng này để khai báo QFile
#include <QCoreApplication> // Thư viện cho ứng dụng Qt
#include <QDir>

ChessServer::ChessServer(QObject *parent) : QTcpServer(parent)
{
    connect(this, &QTcpServer::newConnection, this, &ChessServer::onNewConnection);
}

void ChessServer::startServer(int port)
{
    if (!this->listen(QHostAddress::Any, port)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started on port:" << port;
    }
}

void ChessServer::onNewConnection()
{
    QTcpSocket *clientSocket = nextPendingConnection();
    clients.append(clientSocket);

    connect(clientSocket, &QTcpSocket::readyRead, this, &ChessServer::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &ChessServer::onClientDisconnected);

    qDebug() << "New client connected from:" << clientSocket->peerAddress().toString();
}

void ChessServer::onClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    clients.removeAll(clientSocket);
    clientSocket->deleteLater();
    qDebug() << "Client disconnected";
}

void ChessServer::onReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    QByteArray data = clientSocket->readAll();//Đọc data từ socket

    //Convert sang json
    QJsonDocument jsonData = QJsonDocument::fromJson(data);

    if(jsonData.isObject()) {
        QJsonObject jsonObj = jsonData.object();
        QString type = jsonObj.value("type").toString();
        if(type == "register") {
            QJsonObject json;
            json["type"] = "register_ack";
            json["status"] = "success";
            json["message"] = "Registration successful!";
            QJsonDocument doc(json);
            QByteArray data = doc.toJson(QJsonDocument::Compact);
            if (clientSocket && clientSocket->isWritable()) {
                clientSocket->write(data);
                clientSocket->flush(); // Đảm bảo dữ liệu được gửi ngay lập tức
                qDebug() << "Sent response to client:" << data;
            } else {
                qDebug() << "Client socket is not writable!";
            }
        }

        if (type == "login") {
            QString username = jsonObj.value("username").toString();
            QString password = jsonObj.value("password").toString();
            // Kiểm tra đăng nhập

            bool loginSuccessful = false;
            qDebug() << "Current working directory:" << QDir::currentPath();
            QString path = QCoreApplication::applicationDirPath() + "/account.txt";
            qDebug() << "Account file path:" << path;
            QFile file(path);




            if (!file.exists()) {
                qDebug() << "File account.txt does not exist!";
            }
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    QStringList parts = line.split(" ");
                    if (parts.size() >= 5) {
                        // Kiểm tra nếu username và password khớp
                        if (parts[2] == username && parts[4] == password) {
                            loginSuccessful = true;
                            break;
                        }
                    }
                }
                file.close();
            } else {
                qDebug() << "Unable to open account file!";
            }

            // Tạo JSON phản hồi
            QJsonObject responseJson;
            if (loginSuccessful) {
                responseJson["type"] = "login_ack";
                responseJson["status"] = "success";
                responseJson["message"] = "Login successful!";
            } else {
                responseJson["type"] = "login_ack";
                responseJson["status"] = "failed";
                responseJson["message"] = "Invalid username or password!";
            }

            QJsonDocument doc(responseJson);
            QByteArray responseData = doc.toJson(QJsonDocument::Compact);

            // Gửi phản hồi về cho client
            if (clientSocket && clientSocket->isWritable()) {
                clientSocket->write(responseData);
                clientSocket->flush();  // Đảm bảo dữ liệu được gửi ngay lập tức
                qDebug() << "Sent response to client:" << responseData;
            } else {
                qDebug() << "Client socket is not writable!";
            }
        }
    }
    else {
        qDebug() << "Received data is not a JSON object.";
    }
}
