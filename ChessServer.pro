QT += core network sql

CONFIG += c++17 cmdline lrelease embed_translations

SOURCES += \
    ChessServer.cpp \
    ClientManager.cpp \
    Game.cpp \
    Logger.cpp \
    Matchmaker.cpp \
    authenticationmanager.cpp \
    controllers/AuthController.cpp \
    controllers/MatchController.cpp \
    controllers/RoomController.cpp \
    databasemanager.cpp \
    entities/User.cpp \
    main.cpp \
    repositories/MatchRepository.cpp \
    repositories/RoomRepository.cpp \
    repositories/UserRepository.cpp \
    services/AuthService.cpp \
    services/MatchService.cpp \
    services/RoomService.cpp \
    utils.cpp

HEADERS += \
    ChessServer.h \
    ClientManager.h \
    Game.h \
    Logger.h \
    Matchmaker.h \
    authenticationmanager.h \
    controllers/AuthController.h \
    controllers/MatchController.h \
    controllers/RoomController.h \
    databasemanager.h \
    entities/User.h \
    repositories/MatchRepository.h \
    repositories/RoomRepository.h \
    repositories/UserRepository.h \
    services/AuthService.h \
    services/MatchService.h \
    services/RoomService.h \
    utils.h

DISTFILES += \
    account.txt \
    conanfile.txt \
    config.env \
    log.txt

TRANSLATIONS +=

LIBS += -lssl -lcrypto

INCLUDEPATH += $$PWD/3rdparty/jwt-cpp/include
INCLUDEPATH += $$PWD/3rdparty/json/include
INCLUDEPATH += $$PWD/controllers
INCLUDEPATH += $$PWD/services
INCLUDEPATH += $$PWD/repositories
INCLUDEPATH += $$PWD/entities

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
