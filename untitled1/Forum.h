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
#include <Wt/Http/Response>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Json/Serializer>
#include <Wt/WResource>
#include <Wt/WServer>
#include <iostream>
#include <istream>

class ForumCreate : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~ForumCreate()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        handlePostParams(request);

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("INSERT INTO Forums (name, short_name, user) VALUES (:name, :short_name, :user);");
        query.bindValue(":name", objectRequest["name"].toString());
        query.bindValue(":short_name", objectRequest["short_name"].toString());
        query.bindValue(":user", objectRequest["user"].toString());
        bool ok = query.exec();

        handleResponse();
        //проверка на код и всё такое тут должны быть
        objectResponce["code"] = ok ? 0 : 5;
        bool isForumExist = true;
        objectResponce["response"] = ForumInfo::getForumCreateInfo(objectRequest["name"].toString(), isForumExist);

        prepareOutput();

        response.setStatus(200);

        response.out() << output;
    }
};

class ForumDetails : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~ForumDetails()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        QString related;
        related = related.fromStdString(request.getParameter("related") ? *request.getParameter("related") : " ");
        QString short_name;
        short_name = short_name.fromStdString(request.getParameter("forum") ? *request.getParameter("forum") : " ");

        // QString user;

        bool isForumExist = false;

        handleResponse();
        if (related != " ") {
            responseContent = ForumInfo::getFullForumInfo(short_name, isForumExist);
        } else {
            responseContent = ForumInfo::getForumCreateInfo(short_name, isForumExist);
        }

        objectResponce["response"] = responseContent;

        objectResponce["code"] = isForumExist ? 0 : 1;

        prepareOutput();
        response.setStatus(200);

        response.out() << output;
    };
};

#endif // FORUM_H
