#ifndef USER_H
#define USER_H
#include "Trash.h"
#include <QJsonArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValue>
#include <QSqlQuery>
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
//TODO
#include <HandleTemplates.h>

class UserCreate : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~UserCreate()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        handlePostParams(request);

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("INSERT INTO Users (username, about, name, email, isAnonymous) VALUES (:username, :about, :name, :email, :isAnonymous);");
        query.bindValue(":username", objectRequest["username"].toString());
        query.bindValue(":about", objectRequest["about"].toString());
        query.bindValue(":name", objectRequest["name"].toString());
        query.bindValue(":email", objectRequest["email"].toString());
        query.bindValue(":isAnonymous", objectRequest["isAnonymous"].toBool());
        bool ok = query.exec();

        handleResponse();
        objectResponce["code"] = ok ? 0 : 5;

        responseContent["username"] = objectRequest["username"];
        responseContent["about"] = objectRequest["about"];
        responseContent["name"] = objectRequest["name"].toString();
        responseContent["email"] = objectRequest["email"];
        responseContent["isAnonymous"] = objectRequest["isAnonymous"];
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);

        response.out() << output;
        std::cout << output << "Tut";
    }
};

class UserDetails : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~UserDetails()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        //ПЕРЕПИСАТЬ В USERINFO ВСЕ ЗАПРОСЫ ПРО ЮЗЕРА ((
        //на данный момент это просто захардкожено для прикола
        //Возможно, уже переписал
        QString user;
        user = user.fromStdString(request.getParameter("user") ? *request.getParameter("user") : "");

        QString strGoodReply = Source::getAnswerTemplate();
        bool isUserExist = false;

        handleResponse();
        responseContent = UserInfo::getFullUserInfo(user, isUserExist);
        objectResponce["response"] = responseContent;

        objectResponce["code"] = isUserExist ? 0 : 1;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
        // std::cout << data.toStdString() << std::endl;
    }
};

class UserFollow : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~UserFollow()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        handlePostParams(request);

        handleResponse();

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("INSERT INTO Followers (follower, followee) VALUES (:follower, :followee);");
        query.bindValue(":follower", objectRequest["follower"].toString());
        query.bindValue(":followee", objectRequest["followee"].toString());
        bool ok = query.exec();

        bool isUserExist = false;
        QJsonObject responseContent = UserInfo::getFullUserInfo(objectRequest["follower"].toString(), isUserExist);
        if (isUserExist) {
            objectResponce["code"] = 0;
            objectResponce["response"] = responseContent;
        } else {
            objectResponce["code"] = 1;
            objectResponce["response"] = "error message";
        }

        prepareOutput();
        response.setStatus(200);

        response.out() << output;
    }
};
class UserUpdateProfile : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~UserUpdateProfile()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        handlePostParams(request);

        handleResponse();

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Users SET about=:about, name=:name WHERE email=:user;");
        query.bindValue(":user", objectRequest["user"].toString());
        query.bindValue(":about", objectRequest["about"].toString());
        query.bindValue(":name", objectRequest["name"].toString());
        bool ok = query.exec();

        bool isUserExist = true;
        QJsonObject responseContent = UserInfo::getFullUserInfo(objectRequest["user"].toString(), isUserExist);
        if (isUserExist) {
            objectResponce["code"] = 0;
            objectResponce["response"] = responseContent;
        } else {
            objectResponce["code"] = 1;
            objectResponce["response"] = "error message";
        }

        prepareOutput();

        response.out() << output;
        // std::cout << "DATA____" << data.toStdString();
    }
};

class UserListFollowers : public Wt::WResource, public HandleRequestList {
public:
    virtual ~UserListFollowers()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    { /*
        QString userOrForum;
        userOrForum = userOrForum.fromStdString(request.getParameter("user") ? *request.getParameter("user") : "");
        if (userOrForum == "")

        QString order;
        order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : "desc");
        //0 - magic constant for empty parametr
        QString since_id;
        since_id = userOrForum.fromStdString(request.getParameter("since_id") ? *request.getParameter("since_id") : "");
        QString limit;
        limit = userOrForum.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : "");

        QString str_since;
        QString str_limit;
        QString str_order;
        if (since_id != "")
            str_since = " AND id >= " + since_id;

        if (limit != "")
            str_limit = " LIMIT " + limit + ";";
        if (order != "")
            str_order = " ORDER BY name " + order;

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("SELECT id FROM Users u JOIN Followers f ON  u.email = f.follower WHERE f.followee=:user" + str_since + str_order + str_limit + ";");
        query.bindValue(":user", userOrForum);
        bool ok = query.exec();

        QString strGoodReply = Source::getAnswerTemplateList();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        QJsonObject objectResponce = jsonResponse.object();
        QJsonArray arrayOfUsers;
        bool isUserExist = true; // заглушка
        if (ok) {
            while (query.next()) {
                int id = query.value(0).toInt();
                arrayOfUsers.append(UserInfo::getFullUserInfoID(id, isUserExist));
            }
        }
        objectResponce["response"] = arrayOfUsers;
        QJsonDocument doc(objectResponce);
        QByteArray data = doc.toJson();

        response.setStatus(200);

        response.out() << data.toStdString();*/

        QString user;
        user = user.fromStdString(request.getParameter("user") ? *request.getParameter("user") : "");

        QString order;
        order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : "desc");
        //0 - magic constant for empty parametr
        QString since_id;
        since_id = user.fromStdString(request.getParameter("since_id") ? *request.getParameter("since_id") : "");
        QString limit;
        limit = user.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : "");

        QString str_since;
        QString str_limit;
        QString str_order;
        QString quote = "\"";

        if (since_id != "")
            str_since = " AND id >= " + quote + since_id + quote;

        if (limit != "")
            str_limit = " LIMIT " + limit;
        if (order != "")
            str_order = " ORDER BY name " + order;

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        QString expression;
        expression = "SELECT email FROM Users u JOIN Followers f ON  u.email = f.follower WHERE f.followee=" + quote + user + quote + str_since + str_order + str_limit + ";";
        bool ok = query.exec(expression);

        handleResponse();
        QJsonArray arrayOfFollowers;
        bool isUserExist = true; // заглушка

        if (ok) {
            while (query.next()) {
                QString email = query.value(0).toString();
                QJsonObject jsonObj = UserInfo::getFullUserInfo(email, isUserExist); // assume this has been populated with Json data
                arrayOfFollowers << jsonObj;
            }
        }
        objectResponce["response"] = arrayOfFollowers;

        prepareOutput();

        response.setStatus(200);

        response.out() << output;
    }
};

#endif // USER_H
