#ifndef USER_H
#define USER_H
#include "Trash.h"
#include <HandleTemplates.h>
#include <QJsonArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValue>
#include <QSqlQuery>
#include <QSqlResult>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <Source.h>
#include <UserInfo.h>
#include <Wt/Http/Response>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Json/Serializer>
#include <Wt/WResource>
#include <Wt/WServer>
#include <iostream>


class UserCreate : public Wt::WResource {
public:
    virtual ~UserCreate();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class UserDetails : public Wt::WResource {
public:
    virtual ~UserDetails();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class UserFollow : public Wt::WResource{
public:
    virtual ~UserFollow();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class UserUnFollow : public Wt::WResource {
public:
    virtual ~UserUnFollow();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};
class UserUpdateProfile : public Wt::WResource{
public:
    virtual ~UserUpdateProfile();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class UserListFollowers : public Wt::WResource {
public:
    virtual ~UserListFollowers();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class UserListFollowing : public Wt::WResource {
public:
    virtual ~UserListFollowing();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class UserListPosts : public Wt::WResource {
public:
    virtual ~UserListPosts();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

#endif // USER_H

