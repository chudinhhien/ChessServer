QT += core network sql

CONFIG += c++17 cmdline lrelease embed_translations

SOURCES += \
    authenticationmanager.cpp \
    databasemanager.cpp \
    main.cpp \
    server.cpp \
    user.cpp \
    utils.cpp

HEADERS += \
    authenticationmanager.h \
    databasemanager.h \
    server.h \
    user.h \
    utils.h

DISTFILES += \
    account.txt \
    conanfile.txt \
    config.env

TRANSLATIONS +=

CONFIG += conan_basic_setup
include(conanbuildinfo.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
