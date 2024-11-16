#include "utils.h"
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>

utils::utils() {}

QString utils::getEnvVariableFromFile(const QString &relativeFilePath, const QString &key) {
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    QString filePath = QDir(dir.absolutePath()).filePath(relativeFilePath);
    // Mở file cấu hình
    QFile file(filePath);

    // Kiểm tra xem file có tồn tại không
    if (!file.exists()) {
        qDebug() << "Config file not found!";
        return QString();
    }

    // Mở file để đọc
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open the file for reading!";
        return QString();
    }

    QTextStream in(&file);
    QString line;

    // Đọc từng dòng và tìm biến môi trường
    while (!in.atEnd()) {
        line = in.readLine();

        // Tìm kiếm key trong file
        if (line.startsWith(key + "=")) {
            // Trả về giá trị sau dấu "="
            return line.section('=', 1, 1).trimmed();
        }
    }

    // Nếu không tìm thấy biến môi trường
    qDebug() << "Key not found in config file!";
    return QString();
}
