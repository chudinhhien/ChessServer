#include "UserRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "authenticationmanager.h"

UserRepository::UserRepository(QSqlDatabase db) : db(db) {}

bool UserRepository::createUser(const QString &name, const QString &username, const QString &password) {
    if (userExists(username)) {
        return false;  // Người dùng đã tồn tại
    }

    AuthenticationManager authManager;
    QString salt = authManager.generateSalt();

    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, name, password, salt, elo) VALUES (:username, :name, :password, :salt, :elo)");
    query.bindValue(":name", name);
    query.bindValue(":salt", salt);
    query.bindValue(":username", username);
    query.bindValue(":password", authManager.hashPassword(password, salt));
    query.bindValue(":elo", 1500);

    if (!query.exec()) {
        qWarning() << "Failed to create user:" << query.lastError().text();
        return false;
    }
    return true;
}

bool UserRepository::validateUser(const QString &username, const QString &password) {
    if (username.isEmpty() || password.isEmpty()) {
        qWarning() << "Username or password cannot be empty";
        return false;
    }

    // Truy vấn lấy salt từ cơ sở dữ liệu
    QSqlQuery saltQuery(db);
    saltQuery.prepare("SELECT salt FROM users WHERE username = :username");
    saltQuery.bindValue(":username", username);

    if (!saltQuery.exec()) {
        qWarning() << "Failed to execute salt query:" << saltQuery.lastError().text();
        return false;
    }

    if (!saltQuery.next()) {
        qWarning() << "Username not found:" << username;
        return false;
    }

    QString salt = saltQuery.value(0).toString();
    if (salt.isEmpty()) {
        qWarning() << "Salt is empty for username:" << username;
        return false;
    }

    // Tạo hash từ mật khẩu và salt
    AuthenticationManager authManager;
    QString hashPassword = authManager.hashPassword(password, salt);

    // Truy vấn kiểm tra username và hash password
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", hashPassword);

    if (!query.exec()) {
        qWarning() << "Failed to validate user query:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        int count = query.value(0).toInt();
        return count > 0;  // Trả về true nếu có ít nhất một bản ghi
    }

    qWarning() << "Query did not return any result for user validation";
    return false;
}


bool UserRepository::userExists(const QString &username) {
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec() || !query.next()) {
        qWarning() << "Failed to check if user exists:" << query.lastError().text();
        return false;
    }

    return query.value(0).toInt() > 0;
}

void UserRepository::addOnlinePlayer(QTcpSocket *socket, const QString &username) {
    // Truy vấn thông tin người dùng từ cơ sở dữ liệu
    QSqlQuery query(db);
    query.prepare("SELECT name, elo FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec() || !query.next()) {
        qWarning() << "Failed to fetch user info for username:" << username;
        return;
    }

    // Tạo đối tượng User và thêm vào danh sách onlinePlayers
    User user;
    user.setName(query.value("name").toString());
    user.setUsername(username);
    user.setElo(query.value("elo").toInt());
    user.setState("Online"); // Cập nhật trạng thái

    onlinePlayers[socket] = user;

    qDebug() << "User added to online list:" << username;
}


void UserRepository::removeOnlinePlayer(QTcpSocket *socket) {
    if (onlinePlayers.contains(socket)) {
        qDebug() << "User removed from online list:" << onlinePlayers[socket].getUsername();
        onlinePlayers.remove(socket);
    }
}

QString UserRepository::getUsernameBySocket(QTcpSocket *socket) const {
    if (onlinePlayers.contains(socket)) {
        return onlinePlayers[socket].getUsername();
    }
    return QString(); // Trả về chuỗi rỗng nếu không tìm thấy
}

QList<User> UserRepository::getOnlinePlayers() const {
    return onlinePlayers.values(); // Trả về danh sách User đang online
}

void UserRepository::updatePlayerStatus(QTcpSocket *socket, const QString &status) {
    if (onlinePlayers.contains(socket)) {
        User &user = onlinePlayers[socket]; // Lấy đối tượng User
        user.setState(status); // Cập nhật trạng thái
        qDebug() << "Updated status for user:" << user.getUsername() << "to:" << status;
    } else {
        qWarning() << "Socket not found in online players while updating status.";
    }
}

User UserRepository::getUserByUsername(const QString &username) const {
    QSqlQuery query(db);
    query.prepare("SELECT name, username, elo FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec() || !query.next()) {
        qWarning() << "Failed to fetch user info for username:" << username << query.lastError().text();
        return User(); // Trả về đối tượng User mặc định nếu không tìm thấy
    }

    User user;
    user.setName(query.value("name").toString());
    user.setUsername(query.value("username").toString());
    user.setElo(query.value("elo").toInt());
    user.setState("Online"); // Thiết lập trạng thái mặc định là Online
    return user;
}

QTcpSocket* UserRepository::getSocketByUserName(const QString &username) const {
    // Duyệt qua danh sách onlinePlayers để tìm socket tương ứng với username
    for (auto it = onlinePlayers.begin(); it != onlinePlayers.end(); ++it) {
        if (it.value().getUsername() == username) {
            return it.key(); // Trả về socket tương ứng
        }
    }
    return nullptr; // Không tìm thấy username
}


