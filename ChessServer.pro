QT += core network sql

CONFIG += c++17 cmdline lrelease embed_translations

SOURCES += \
    ChessServer.cpp \
    ClientManager.cpp \
    Game.cpp \
    Matchmaker.cpp \
    authenticationmanager.cpp \
    controllers/MatchController.cpp \
    databasemanager.cpp \
    main.cpp \
    repositories/MatchRepository.cpp \
    server.cpp \
    services/MatchService.cpp \
    user.cpp \
    utils.cpp

HEADERS += \
    ChessServer.h \
    ClientManager.h \
    Game.h \
    Matchmaker.h \
    authenticationmanager.h \
    controllers/MatchController.h \
    databasemanager.h \
    repositories/MatchRepository.h \
    server.h \
    services/MatchService.h \
    user.h \
    utils.h

DISTFILES += \
    account.txt \
    conanfile.txt \
    config.env

TRANSLATIONS +=

LIBS += -lssl -lcrypto

INCLUDEPATH += $$PWD/3rdparty/jwt-cpp/include
INCLUDEPATH += $$PWD/3rdparty/json/include
INCLUDEPATH += $$PWD/controllers
INCLUDEPATH += $$PWD/services
INCLUDEPATH += $$PWD/repositories

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
