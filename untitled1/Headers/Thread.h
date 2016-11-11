#ifndef THREAD_H
#define THREAD_H
#include "ThreadInfo.h"
#include "Trash.h"
#include "User.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValue>
#include <QRegExp>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <UserInfo.h>
#include <Wt/Http/Response>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Json/Serializer>
#include <Wt/WResource>
#include <Wt/WServer>
#include <iostream>

class ThreadCreate : public Wt::WResource {
public:
    virtual ~ThreadCreate();;

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};
class ThreadDetails : public Wt::WResource {
public:
    virtual ~ThreadDetails();;

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};
class ThreadClose : public Wt::WResource {
public:
    virtual ~ThreadClose();;

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};
class ThreadOpen : public Wt::WResource {
public:
    virtual ~ThreadOpen();;

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class ThreadRemove : public Wt::WResource {
public:
    virtual ~ThreadRemove();

protected:
    //тут должен быть роллбек
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class ThreadRestore : public Wt::WResource {
public:
    virtual ~ThreadRestore();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class ThreadUpdate : public Wt::WResource {
public:
    virtual ~ThreadUpdate();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class ThreadVote : public Wt::WResource {
public:
    virtual ~ThreadVote();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class ThreadSubscribe : public Wt::WResource {
public:
    virtual ~ThreadSubscribe();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class ThreadUnSubscribe : public Wt::WResource {
public:
    virtual ~ThreadUnSubscribe();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};
class ThreadList : public Wt::WResource {
public:
    virtual ~ThreadList();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class ThreadListPost : public Wt::WResource {
public:
    virtual ~ThreadListPost();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};
#endif // THREAD_H

