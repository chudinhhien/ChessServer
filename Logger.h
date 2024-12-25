#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QMutex>

class Logger {
public:
    static Logger& instance();
    void log(const QString& message, const QString& logFilePath = "/home/lenovo/ChessServer/log.txt");

private:
    Logger();  // Constructor private để Singleton
    ~Logger();
    QMutex mutex;
};

void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

#endif // LOGGER_H
