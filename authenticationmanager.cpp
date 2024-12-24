// AuthenticationManager.cpp
#include "authenticationmanager.h"
#include <databasemanager.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QProcessEnvironment>
#include <utils.h>
#include <nlohmann/json.hpp>
#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/traits.h>
#include <QJsonDocument>
using traits = jwt::traits::nlohmann_json;

AuthenticationManager::AuthenticationManager(QObject *parent) : QObject(parent) {
    // Khởi tạo kết nối cơ sở dữ liệu ở đây hoặc qua một DatabaseManager
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
}

QString AuthenticationManager::generateSalt(int length) {
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    QString salt;
    for(int i=0; i<length; ++i) {
        int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
        salt += possibleCharacters.at(index);
    }
    return salt;
}

QString AuthenticationManager::hashPassword(const QString &password, const QString &salt) {
    QByteArray combined = (password + salt).toUtf8();
    QByteArray hash = QCryptographicHash::hash(combined, QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool AuthenticationManager::registerUser(User &user, QString &errorMessage) {
    user.salt = generateSalt();
    user.password = hashPassword(user.password, user.salt);

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", user.username);
    if (!query.exec()) {
        errorMessage = "Database query failed.";
        return false;
    }

    if (query.next() && query.value(0).toInt() > 0) {
        errorMessage = "Username already exists.";
        return false;
    }

    query.prepare("INSERT INTO users (username, password, salt, name, elo) VALUES (:username, :password, :salt, :name, :elo)");
    query.bindValue(":username", user.username);
    query.bindValue(":password", user.password);
    query.bindValue(":salt", user.salt);
    query.bindValue(":name", user.name);
    query.bindValue(":elo", user.elo);

    if (!query.exec()) {
        errorMessage = "Error saving data.";
        return false;
    }

    return true;
}

bool AuthenticationManager::loginUser(QTcpSocket *clientSocket, const QString &username, const QString &password, QString &token, QString &errorMessage) {
    QSqlQuery query;
    query.prepare("SELECT password, salt, name, elo FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        errorMessage = "Database query failed.";
        return false;
    }

    if (!query.next()) {
        errorMessage = "Invalid username or password.";
        return false;
    }

    QString storedHash = query.value("password").toString();
    QString salt = query.value("salt").toString();
    QString name = query.value("name").toString();
    QString elo = query.value("elo").toString();

    qDebug() << elo << "\n";

    QJsonObject res;

    QString hashedInput = hashPassword(password, salt);
    if (hashedInput != storedHash) {
        errorMessage = "Invalid username or password.";
        return false;
    }

    res["type"] = "login_ack";
    res["status"] = "success";
    res["message"] = "Login successful!";
    res["name"] = name;
    res["username"] = username;
    res["password"] = password;
    res["elo"] = elo;

    QJsonDocument doc(res);
    QByteArray data = doc.toJson();

    clientSocket->write(data);
    clientSocket->flush();

    // Tạo token JWT
    try {
        QString secretKey = utils::getEnvVariableFromFile("config.env", "JWT_SECRET_KEY");
        auto jwtToken = jwt::create<traits>()
        .set_issuer("ChessServer")
            .set_type("JWS")
            .set_payload_claim("username", username.toStdString())
            .set_payload_claim("name", name.toStdString())
            .set_issued_at(std::chrono::system_clock::now())
            .set_expires_at(std::chrono::system_clock::now() + std::chrono::minutes{60})
            .sign(jwt::algorithm::hs256{secretKey.toStdString()});

        token = QString::fromStdString(jwtToken);
    }
    catch (const std::exception &e) {
        errorMessage = "Token generation failed.";
        return false;
    }

    return true;
}
