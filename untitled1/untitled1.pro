TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG += qt
QT += sql
CONFIG += c++14
QMAKE_CXXFLAGS += -std=c++1y


SOURCES += main.cpp

LIBS += -L/usr/lib -lwt -lwthttp -lwtdbo -lwtdbosqlite3 -I/usr/local/include
LIBS += -L/usr/local/lib -lwthttp -lwt -lboost_regex -lboost_signals
LIBS += -lboost_system -lboost_thread -lboost_filesystem -lboost_date_time -lpthread -lcrypt


HEADERS += \
    DB.h \
    BdWrapper.h \
    Router.h \
    Forum.h \
    User.h \
    UserInfo.h \
    Trash.h \
    Thread.h \
    Source.h \
    ThreadInfo.h \
    PostInfo.h \
    Post.h \
    HandleTemplates.h \
    ForumInfo.h

