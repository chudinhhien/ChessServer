#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QThread>

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    static DatabaseManager& instance();
    bool openConnection();
    void createTableForUser();
    void createTableForMatches();
    QSqlDatabase getDatabase();
    void cleanupConnection();

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
