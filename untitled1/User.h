#ifndef USER_H
#define USER_H
#include "Trash.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValue>
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

class UserCreate : public Wt::WResource {
public:
    virtual ~UserCreate()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        QString params = LineAnalyze::getRequestBody(request);
        QJsonDocument jsonRequest = QJsonDocument::fromJson(params.toUtf8());

        QJsonObject objectRequest = jsonRequest.object();

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("INSERT INTO Users (username, about, name, email, isAnonymous) VALUES (:username, :about, :name, :email, :isAnonymous);");
        query.bindValue(":username", objectRequest["username"].toString());
        query.bindValue(":about", objectRequest["about"].toString());
        query.bindValue(":name", objectRequest["name"].toString());
        query.bindValue(":email", objectRequest["email"].toString());
        query.bindValue(":isAnonymous", objectRequest["isAnonymous"].toBool());
        bool ok = query.exec();

        QString strGoodReply = Source::getAnswerTemplate();

        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        QJsonObject objectResponce = jsonResponse.object();

        QJsonObject jsonArray = objectResponce["response"].toObject();
        objectResponce["code"] = ok?0:5;

        jsonArray["username"] = objectRequest["username"];
        jsonArray["about"] = objectRequest["about"];
        jsonArray["name"] = objectRequest["name"].toString();
        jsonArray["email"] = objectRequest["email"];
        jsonArray["isAnonymous"] = objectRequest["isAnonymous"];
        objectResponce["response"] = jsonArray;


        QJsonDocument doc(objectResponce);
        QByteArray data = doc.toJson();
        QJsonDocument doc2(jsonArray);
        QByteArray data2 = doc2.toJson();

        response.setStatus(200);

        response.out() << data.toStdString();
        std::cout<<data2.toStdString();
    }
};

class UserDetails : public Wt::WResource {
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

        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        QJsonObject objectResponce = jsonResponse.object();
        QJsonObject jsonArray = UserInfo::getFullUserInfo(user, isUserExist);
        if (!isUserExist) {
            objectResponce["code"] = 1;
        } else {
            objectResponce["code"] = 0;
            objectResponce["response"] = jsonArray;
        }
        QJsonDocument doc(objectResponce); //если пользователя такого нет, то отдаем код 1 и дефолтные значения
        QByteArray data = doc.toJson();
        response.setStatus(200);
        response.out() << data.toStdString();
        std::cout<<data.toStdString()<<std::endl;
    }
};

class UserFollow : public Wt::WResource {
public:
    virtual ~UserFollow()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        QString params = LineAnalyze::getRequestBody(request);

        QJsonDocument jsonRequest = QJsonDocument::fromJson(params.toUtf8());

        QJsonObject objectRequest = jsonRequest.object();

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("INSERT INTO Followers (follower, followee) VALUES (:follower, :followee);");
        query.bindValue(":follower", objectRequest["follower"].toString());
        query.bindValue(":followee", objectRequest["followee"].toString());
        bool ok = query.exec();

        QJsonObject objectResponce = objectRequest;
        bool isUserExist = false;
        QJsonObject jsonArray = UserInfo::getFullUserInfo(objectRequest["follower"].toString(), isUserExist);
        if (isUserExist) {
            objectResponce["code"] = 0;
            objectResponce["response"] = jsonArray;
        }
        else {
            objectResponce["code"] = 1;
            objectResponce["response"] = "error message";
        }

        QJsonDocument doc(objectResponce);
        QByteArray data = doc.toJson();

        response.setStatus(200);

        response.out() << data.toStdString();
    }
};
class UserUpdateProfile: public Wt::WResource {
public:
    virtual ~UserUpdateProfile()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        QString params = LineAnalyze::getRequestBody(request);

        QJsonDocument jsonRequest = QJsonDocument::fromJson(params.toUtf8());
        QJsonObject objectRequest = jsonRequest.object();

        QString strGoodReply = Source::getAnswerTemplate();

        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        QJsonObject objectResponce = jsonResponse.object();

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Users SET about=:about, name=:name WHERE email=:user;");
        query.bindValue(":user", objectRequest["user"].toString());
        query.bindValue(":about", objectRequest["about"].toString());
        query.bindValue(":name", objectRequest["name"].toString());
        bool ok = query.exec();
        bool isUserExist = true;
        QJsonObject jsonArray = UserInfo::getFullUserInfo(objectRequest["user"].toString(), isUserExist);
        if (isUserExist) {
            objectResponce["code"] = 0;
            objectResponce["response"] = jsonArray;
        }
        else {
            objectResponce["code"] = 1;
            objectResponce["response"] = "error message";
        }

        QJsonDocument doc(objectResponce);
        QByteArray data = doc.toJson();

        response.setStatus(200);

        response.out() << data.toStdString();
    }
};

#endif // USER_H
