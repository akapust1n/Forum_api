#ifndef FORUM_H
#define FORUM_H
#include "ForumInfo.h"
#include "Trash.h"
#include <HandleTemplates.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <ThreadInfo.h>
#include <Wt/Http/Response>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Json/Serializer>
#include <Wt/WResource>
#include <Wt/WServer>
#include <iostream>
#include <istream>
#include <zdb/zdb.h>
#include "Wt/WSignal"
//extern     Connection_T pool1;

class ForumCreate : public Wt::WResource {
public:
    virtual ~ForumCreate();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class ForumDetails : public Wt::WResource {
public:
    virtual ~ForumDetails();;

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);;
};

class ForumListPosts : public Wt::WResource {
public:
    virtual ~ForumListPosts();;

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class ForumListThreads : public Wt::WResource{
public:
    virtual ~ForumListThreads();;

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class ForumListUsers : public Wt::WResource{
public:
    virtual ~ForumListUsers();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};
#endif // FORUM_H

