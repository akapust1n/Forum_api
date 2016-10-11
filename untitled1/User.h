#ifndef USER_H
#define USER_H
#include "Encoding.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QVariant>
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
        QString params = "";
        //иногда падает с ошибкой почему-то
        try {
            std::string line = request.getParameterMap().begin()->first;
            std::cout<<"HUI"<<line<<std::endl;
            if (line.size() != 0) {
                params = Encoding::fixUnicode(line);
            }
        } catch (...) {
        };

        QJsonDocument jsonRequest = QJsonDocument::fromJson(params.toUtf8());

        QJsonObject objectRequest = jsonRequest.object();
        if (objectRequest["isAnonymous"] == "")
            objectRequest["isAnonymous"] = false;

        QString strGoodReply = "{"
                               "\"code\": 0,"
                               "\"response\": {"
                               "\"about\": \"hello im user1\","
                               "\"email\": \"example@mail.ru\","
                               "\"id\": 1,"
                               "\"isAnonymous\": false,"
                               "\"name\": \"John\","
                               "\"username\": \"user1\""
                               " }"
                               "}";

        QSqlQuery query;
        query.prepare("INSERT INTO Forums (username, about, name, email, isAnonymous) VALUES (:username, :about, :name, :email, :isAnonymous);");
        query.bindValue(":username", objectRequest["username"].toString());
        query.bindValue(":about", objectRequest["about"].toString());
        query.bindValue(":name", objectRequest["name"].toString());
        query.bindValue(":email", objectRequest["email"].toString());
        query.bindValue(":isAnonymous", objectRequest["isAnonymous"].toBool());

        bool ok = query.exec();
        std::cout<<"OOOOOOOOOOOOOOK___"<<ok<<std::endl;

        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        QJsonObject objectResponce = jsonResponse.object();

        QJsonObject jsonArray = objectResponce["response"].toObject();
        if (!ok)
            objectResponce["code"] = 5;
        jsonArray["username"] = objectRequest["username"];
        jsonArray["about"] = objectRequest["about"];
        jsonArray["name"] = objectRequest["name"];
        jsonArray["email"] = objectRequest["email"];
        jsonArray["isAnonymous"] = objectRequest["isAnonymous"];
        objectResponce["response"] = jsonArray;

        QJsonDocument doc(objectResponce);
        QByteArray data = doc.toJson();

        response.setStatus(200);

        response.out() << data.toStdString();
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
        std::string user = request.getParameter("user") ? *request.getParameter("user") : "";

        QString strGoodReply = "{"
                               "\"code\": 0,"
                               "\"response\": {"
                               "\"about\": \"hello im user1\","
                               "\"email\": \"example@mail.ru\","
                               "\"followers\": ["
                               "\"example3@mail.ru\""
                               "],"
                               "\"following\": ["
                               "\"example3@mail.ru\""
                               "],"
                               "\"id\": 1,"
                               "\"isAnonymous\": false,"
                               "\"name\": \"John\","
                               "\"subscriptions\": ["
                               "4"
                               "],"
                               "\"username\": \"user1\""
                               " }"
                               " }"
                               "}"
                               "}";
        "}";
        QSqlQuery query;
        query.prepare("SELECT * FROM Users WHERE email=:user;");
        query.bindValue(":user", QString::fromStdString(user));
        bool ok= query.exec();

        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        QJsonObject objectResponce = jsonResponse.object();
        QJsonObject jsonArray = objectResponce["response"].toObject();
        if(ok){
            jsonArray["username"] = query.value(0).toString();
            jsonArray["about"] = query.value(1).toString();
            jsonArray["name"] = query.value(2).toString();
            jsonArray["email"] = query.value(3).toString();
            jsonArray["isAnonymous"] = query.value(4).toBool();
            QJsonDocument doc(objectResponce);
            QByteArray data = doc.toJson();

            response.setStatus(200);

            response.out() << data.toStdString();
        }
    }
};
#endif // USER_H
