TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG += qt
QT += sql
CONFIG += c++14
QMAKE_CXXFLAGS += -std=c++1y


LIBS += -L/usr/lib -lwt -lwthttp -lwtdbo -lwtdbosqlite3  -lzdb -I/usr/local/include -I/usr/include/zdb
LIBS += -L/usr/local/lib -lwthttp -lwt -lboost_regex -lboost_signals
LIBS += -lboost_system -lboost_thread -lboost_filesystem -lboost_date_time -lpthread -lcrypt

INCLUDEPATH += ./Headers
CONFIG += no_keywords


unix|win32: LIBS += -lzdb

HEADERS += \
    Headers/DB.h \
    Headers/Forum.h \
    Headers/ForumInfo.h \
    Headers/HandleTemplates.h \
    Headers/Post.h \
    Headers/PostInfo.h \
    Headers/Router.h \
    Headers/Source.h \
    Headers/Thread.h \
    Headers/ThreadInfo.h \
    Headers/Trash.h \
    Headers/User.h \
    Headers/UserInfo.h

SOURCES += \
    Sources/DB.cpp \
    Sources/main.cpp \
    Sources/Forum.cpp \
    Sources/UserInfo.cpp \
    Sources/ForumInfo.cpp \
    Sources/HandleTemplates.cpp \
    Sources/Post.cpp \
    Sources/PostInfo.cpp \
    Sources/Thread.cpp \
    Sources/ThreadInfo.cpp \
    Sources/User.cpp
