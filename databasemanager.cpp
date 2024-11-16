#include "databasemanager.h"
#include "utils.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {
    QString host = utils::getEnvVariableFromFile("config.env", "host");
    QString dbName = utils::getEnvVariableFromFile("config.env", "dbName");
    QString user = utils::getEnvVariableFromFile("config.env", "username");
    QString password = utils::getEnvVariableFromFile("config.env", "password");

    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(host);
    db.setDatabaseName(dbName);
    db.setUserName(user);
    db.setPassword(password);
}

DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::openConnection() {
    if (!db.open()) {
        qDebug() << "Connection failed:" << db.lastError().text();
        return false;
    }
    qDebug() << "Connected to the database!";
    return true;
}

void DatabaseManager::createTableForUser() {
    QSqlQuery query(db);
    QString createTableQuery = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INT AUTO_INCREMENT PRIMARY KEY,
            username VARCHAR(255) NOT NULL UNIQUE,
            name VARCHAR(255),
            password VARCHAR(255),
            salt VARCHAR(255) NOT NULL,
            elo FLOAT
        )
    )";
    if (!query.exec(createTableQuery)) {
        qDebug() << "Failed to create table:" << query.lastError().text();
    } else {
        qDebug() << "Table created successfully!";
    }
}

QSqlDatabase DatabaseManager::getDatabase() const {
    return db;
}
