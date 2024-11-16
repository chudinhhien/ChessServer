#ifndef USER_H
#define USER_H

#include <QString>
#include <QJsonObject>

class User {
public:
    // Các thuộc tính
    QString username;
    QString password;
    QString name;
    QString salt;
    int elo;

    // Constructor từ QJsonObject
    User(const QJsonObject &jsonObj);

    // Phương thức chuyển User thành QJsonObject
    QJsonObject toJson() const;
};

#endif // USER_H
