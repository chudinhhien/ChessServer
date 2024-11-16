#ifndef UTILS_H
#define UTILS_H

#include <QString>

class utils {
public:
    utils();  // Constructor
    static QString getEnvVariableFromFile(const QString &filePath, const QString &key);  // Lấy biến môi trường từ file
};

#endif // UTILS_H
