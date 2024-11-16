// AuthenticationManager.h
#ifndef AUTHENTICATIONMANAGER_H
#define AUTHENTICATIONMANAGER_H

#include <QObject>
#include <QString>
#include "user.h"

class AuthenticationManager : public QObject {
    Q_OBJECT
public:
    explicit AuthenticationManager(QObject *parent = nullptr);
    bool registerUser(User &user, QString &errorMessage);
    bool loginUser(const QString &username, const QString &password, QString &token, QString &errorMessage);

private:
    QString generateSalt(int length = 16);
    QString hashPassword(const QString &password, const QString &salt);
    // Thêm các thành phần cần thiết khác như quản lý cơ sở dữ liệu
};

#endif // AUTHENTICATIONMANAGER_H
