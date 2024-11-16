#include "user.h"

// Constructor từ QJsonObject
User::User(const QJsonObject &jsonObj) {
    username = jsonObj.value("username").toString();
    password = jsonObj.value("password").toString();
    name = jsonObj.value("name").toString();
    salt = jsonObj.contains("salt") ? jsonObj.value("salt").toString() : "";
    elo = jsonObj.contains("elo") ? jsonObj.value("elo").toInt() : 1500;
}

// Phương thức chuyển User thành QJsonObject
QJsonObject User::toJson() const {
    QJsonObject obj;
    obj["username"] = username;
    obj["password"] = password; // Lưu ý: không nên gửi password trong ứng dụng thực tế
    obj["name"] = name;
    obj["salt"] = salt;
    obj["elo"] = elo;
    return obj;
}
