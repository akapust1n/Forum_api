#ifndef POST_H
#define POST_H
#include "PostInfo.h"
#include "ThreadInfo.h"
#include "Trash.h"
#include "User.h"
#include <PostInfo.h>
#include <PostInfo.h>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValue>
#include <QRegExp>
#include <QSqlError>
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
#include "ForumInfo.h"
#include "ThreadInfo.h"

class PostCreate : public Wt::WResource{
public:
    virtual ~PostCreate();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class PostDetails : public Wt::WResource {
public:
    virtual ~PostDetails();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class PostRemove : public Wt::WResource{
public:
    virtual ~PostRemove();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class PostRestore : public Wt::WResource {
public:
    virtual ~PostRestore();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class PostUpdate : public Wt::WResource{
public:
    virtual ~PostUpdate();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class PostVote : public Wt::WResource{
public:
    virtual ~PostVote();;

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class PostList : public Wt::WResource{
public:
    virtual ~PostList();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

#endif // POST_H

