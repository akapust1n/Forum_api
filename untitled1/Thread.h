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

class ThreadCreate : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~ThreadCreate()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        handlePostParams(request);
        if (objectRequest["isDeleted"] == "")
            objectRequest["isDeleted"] = false;

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("INSERT INTO Threads (forum, title, isClosed, user, date, message, slug, isDeleted) VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
        query.bindValue(0, objectRequest["forum"].toString());
        query.bindValue(1, objectRequest["title"].toString());
        query.bindValue(2, objectRequest["isClosed"].toBool());
        query.bindValue(3, objectRequest["user"].toString());
        query.bindValue(4, objectRequest["date"].toString());
        query.bindValue(5, objectRequest["message"].toString());
        query.bindValue(6, objectRequest["slug"].toString());
        query.bindValue(7, objectRequest["isDeleted"].toBool());
        bool ok = query.exec();

        handleResponse();
        objectResponce["code"] = ok ? 0 : 4;

        responseContent["forum"] = objectRequest["forum"];
        responseContent["title"] = objectRequest["title"];
        responseContent["isClosed"] = objectRequest["isClosed"].toString();
        responseContent["user"] = objectRequest["user"];
        responseContent["date"] = objectRequest["date"];
        responseContent["message"] = objectRequest["message"].toString();
        responseContent["user"] = objectRequest["user"];
        responseContent["slug"] = objectRequest["slug"];
        responseContent["isDeleted"] = objectRequest["isDeleted"];
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);

        response.out() << output;
        std::cout << output << "Tut";
    }
};
class ThreadDetails : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~ThreadDetails()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        //ПЕРЕПИСАТЬ В USERINFO ВСЕ ЗАПРОСЫ ПРО ЮЗЕРА ((
        //на данный момент это просто захардкожено для прикола
        //Возможно, уже переписал
        QString thread;
        thread = thread.fromStdString(request.getParameter("thread") ? *request.getParameter("thread") : "");
        int thread_id = thread.toInt();
        QString relatedArray[2]; //0-user, 1-forum
        relatedArray[0] = "";
        relatedArray[1] = "";

        auto temp = request.getParameterValues("related");

        for (auto i = temp.begin(); i != temp.end(); i++) {
            if ((*i) == "user")
                relatedArray[0] = "user";
            else
                relatedArray[1] = "forum";
        }

        // QString user;
        // user= thread.fromStdString(request.getParameter("thread") ? *request.getParameter("thread") : "");

        bool isThreadExist = false;

        handleResponse();
        responseContent = ThreadInfo::getFullThreadInfo(thread_id, isThreadExist);

        //  isUserExist костыль, надо как-то перерабыватывать функцию
        if (relatedArray[0] != "") {
            responseContent["user"] = UserInfo::getFullUserInfo(responseContent["user"].toString(), isThreadExist);
        }
        if (relatedArray[1] != "") { //TODO
            // responseContent["user"] = UserInfo::getFullUserInfo(responseContent["user"], isUserExist);
        }
        objectResponce["response"] = responseContent;

        objectResponce["code"] = isThreadExist ? 0 : 1;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
        //std::cout << data.toStdString() << std::endl;
    }
};
class ThreadClose : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~ThreadClose()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {

        handlePostParams(request);
        handleResponse();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Threads SET isClosed=true WHERE id=?;");
        query.bindValue(0, objectRequest["thread"].toString());
        bool ok = query.exec();
        responseContent["thread"] = objectRequest["thread"];
        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
    }
};

class ThreadList : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~ThreadList()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {

    }
};
#endif // THREAD_H
