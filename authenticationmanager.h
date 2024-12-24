// AuthenticationManager.h
#ifndef AUTHENTICATIONMANAGER_H
#define AUTHENTICATIONMANAGER_H

#include <QObject>
#include <QString>
#include "User.h"
#include <QTcpSocket>

class AuthenticationManager : public QObject {
    Q_OBJECT
public:
    explicit AuthenticationManager(QObject *parent = nullptr);
    bool registerUser(User &user, QString &errorMessage);
    bool loginUser(QTcpSocket *clientSocket,const QString &username, const QString &password, QString &token, QString &errorMessage);
    QString hashPassword(const QString &password, const QString &salt);
    QString generateSalt(int length = 16);

private:
    // Thêm các thành phần cần thiết khác như quản lý cơ sở dữ liệu
};

#endif // AUTHENTICATIONMANAGER_H
