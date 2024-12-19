#include "ClientManager.h"
#include <QDebug>
#include <QJsonDocument>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "databasemanager.h"
#include "authenticationmanager.h"

ClientManager::ClientManager(QObject *parent) : QObject(parent) {
    // Constructor logic if necessary
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    connect(this, &ClientManager::sendMessageToSocket, this, [](QTcpSocket *socket, QByteArray data) {
        if (socket->state() == QAbstractSocket::ConnectedState) {
            qDebug().noquote() << data << '\n';
            socket->write(data);
            socket->flush();
        } else {
            qDebug() << "Socket is not in connected state, cannot write data.";
        }
    });
}

ClientManager::~ClientManager() {
    // Destructor logic if necessary
}

void ClientManager::handleUpdateProfileAck(QTcpSocket *socket, const QJsonObject &jsonObj) {
    QString username = jsonObj["username"].toString();
    QString name = jsonObj["name"].toString();
    QJsonObject tmp;
    tmp["type"] = "update_profile_ack";

    QSqlQuery query;
    query.prepare("UPDATE users SET name = :name WHERE username = :username");
    query.bindValue(":name", name);
    query.bindValue(":username", username);

    QString status, message;
    if (query.exec()) {
        tmp["status"] = "success";
        tmp["message"] = "Profile updated successfully.";
    } else {
        tmp["status"] = "failure";
        tmp["message"] = "Failed to update profile: " + query.lastError().text();
        tmp["name"] = query.value("name").toString();
    }

    qDebug() << "Profile update query result:" << status << message;

    // Send response back to client
    QJsonDocument doc(tmp);
    QByteArray res = doc.toJson(QJsonDocument::Indented);
    emit sendMessageToSocket(socket, res);
}

void ClientManager::handleChangePasswordAck(QTcpSocket *socket, const QJsonObject &jsonObj) {
    QString username = jsonObj["username"].toString();
    QString newPassword = jsonObj["password"].toString();

    QJsonObject tmp;
    tmp["type"] = "change_password_ack";

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "Database connection is invalid or not open.";
        tmp["status"] = "failure";
        tmp["message"] = "Database connection is invalid or not open.";
        QJsonDocument doc(tmp);
        QByteArray res = doc.toJson(QJsonDocument::Indented);
        emit sendMessageToSocket(socket, res);
        return;
    }

    QSqlQuery query(db);
    AuthenticationManager authManager;
    query.prepare("SELECT salt, password FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec() || !query.next()) {
        qWarning() << "Failed to fetch user data: " << query.lastError().text();
        tmp["status"] = "failure";
        tmp["message"] = "Failed to fetch user data: " + query.lastError().text();
        QJsonDocument doc(tmp);
        QByteArray res = doc.toJson(QJsonDocument::Indented);
        emit sendMessageToSocket(socket, res);
        return;
    }

    QString salt = query.value("salt").toString();

    // Hash new password with the same salt
    QString hashedNewPassword = authManager.hashPassword(newPassword, salt);

    // Update password in database
    query.prepare("UPDATE users SET password = :newPassword WHERE username = :username");
    query.bindValue(":newPassword", hashedNewPassword);
    query.bindValue(":username", username);

    QString status, message;
    if (query.exec()) {
        status = "success";
        message = "Password changed successfully.";
    } else {
        status = "failure";
        message = "Failed to change password: " + query.lastError().text();
    }

    qDebug() << "Password change query result:" << status << message;

    tmp["status"] = "success";
    tmp["message"] = "Update password successful !";
    QJsonDocument doc(tmp);
    QByteArray res = doc.toJson(QJsonDocument::Indented);
    // Send response back to client
    emit sendMessageToSocket(socket, res);
}

void handleCreateRoomAck(QTcpSocket *socket, const QJsonObject &jsonObj) {
    QString username = jsonObj["username"].toString();
}
