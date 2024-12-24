#ifndef ROOMSERVICE_H
#define ROOMSERVICE_H

#include "RoomRepository.h"
#include <QString>

class RoomService {
public:
    explicit RoomService(RoomRepository *repository);

    bool createRoom(const QString &username, QString &roomId, QString &message);

private:
    RoomRepository *repository;
};

#endif // ROOMSERVICE_H
