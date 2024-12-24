#ifndef USER_H
#define USER_H

#include <QString>

class User {
public:
    User(); // Constructor mặc định
    User(const QString &name, const QString &username, int elo, const QString &state);

    // Getters
    QString getName() const;
    QString getUsername() const;
    int getElo() const;
    QString getState() const;

    // Setters
    void setName(const QString &name);
    void setUsername(const QString &username);
    void setElo(int elo);
    void setState(const QString &state);

private:
    QString name;       // Tên người dùng
    QString username;   // Tên đăng nhập
    int elo;            // Điểm Elo
    QString state;      // Trạng thái (Online, In Match, ...)
};

#endif // USER_H
