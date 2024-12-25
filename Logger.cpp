#include "Logger.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutexLocker>

Logger::Logger() {}

Logger::~Logger() {}

Logger& Logger::instance() {
    static Logger loggerInstance;
    return loggerInstance;
}

void Logger::log(const QString& message, const QString& logFilePath) {
    QMutexLocker locker(&mutex); // Đảm bảo thread-safe
    QFile file(logFilePath);

    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        out << "[" << timestamp << "] " << message << "\n";
        file.close();
    }
}

// Hàm xử lý log tùy chỉnh cho qDebug, qWarning, qCritical, qFatal
void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    QString logMessage;
    Q_UNUSED(context);

    // Loại log (Debug, Info, Warning, Critical, Fatal)
    switch (type) {
    case QtDebugMsg:
        logMessage = QString("DEBUG: %1").arg(msg);
        break;
    case QtInfoMsg:
        logMessage = QString("INFO: %1").arg(msg);
        break;
    case QtWarningMsg:
        logMessage = QString("WARNING: %1").arg(msg);
        break;
    case QtCriticalMsg:
        logMessage = QString("CRITICAL: %1").arg(msg);
        break;
    case QtFatalMsg:
        logMessage = QString("FATAL: %1").arg(msg);
        break;
    }

    // Ghi vào log file
    Logger::instance().log(logMessage);

    // In ra console (nếu cần)
    fprintf(stderr, "%s\n", logMessage.toLocal8Bit().constData());

    // Nếu là Fatal, thoát chương trình
    if (type == QtFatalMsg)
        abort();
}

