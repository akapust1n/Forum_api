#ifndef POST_H
#define POST_H
#include "PostInfo.h"
#include "ThreadInfo.h"
#include "Trash.h"
#include "User.h"
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

class PostCreate : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~PostCreate()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        handlePostParams(request);
        if (objectRequest["isDeleted"] == "")
            objectRequest["isDeleted"] = false;

        if (objectRequest["parent"] == "")
            objectRequest["parent"] = false;

        if (objectRequest["isApproved"] == "")
            objectRequest["isApproved"] = false;

        if (objectRequest["isHighlighted"] == "")
            objectRequest["isHighlighted"] = false;

        if (objectRequest["isEdited"] == "")
            objectRequest["isEdited"] = true;

        if (objectRequest["isSpam"] == "")
            objectRequest["isSpam"] = false;

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("INSERT INTO Posts (date, thread_id, message, user, forum, parent, isApproved, isHighlighted, isEdited, isSpam, isDeleted) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
        query.bindValue(0, objectRequest["date"].toString());
        query.bindValue(1, objectRequest["thread"].toInt());
        query.bindValue(2, objectRequest["message"].toString());
        query.bindValue(3, objectRequest["user"].toString());
       // if (objectRequest["forum"] != QJsonValue::Null)
            query.bindValue(4, objectRequest["forum"].toString());
      //  else
       //     query.bindValue(4, QJsonValue::Null);

       if (objectRequest["parent"] !=QJsonValue::Null)
            query.bindValue(5, objectRequest["parent"].toInt());
        else
           query.bindValue(5, QVariant::Int);
        query.bindValue(6, objectRequest["isApproved"].toBool());
        query.bindValue(7, objectRequest["isHighlighted"].toBool());
        query.bindValue(8, objectRequest["isEdited"].toBool());
        query.bindValue(9, objectRequest["isSpam"].toBool());
        query.bindValue(10, objectRequest["isDeleted"].toBool());
        bool ok = query.exec();
        auto tt = query.lastError().text();

        handleResponse();
        if (ok) {
            objectResponce["code"] = 0;
            bool isThreadExist = true; //костыль(
            int last_id = query.lastInsertId().toInt();
            objectResponce["response"] = PostInfo::getFullPostInfo(query.lastInsertId().toInt(), isThreadExist);
        }

        else {
            objectResponce["code"] = 4;
        }

        prepareOutput();
        response.setStatus(200);

        response.out() << output;
        // std::cout << output << "Tat";
    }
};

class PostDetails : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~PostDetails()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        QString post;
        post = post.fromStdString(request.getParameter("post") ? *request.getParameter("post") : "");
        int post_id = post.toInt();
        QString relatedArray[3]; //0-user, 1-forum,3 -thread
        relatedArray[0] = "";
        relatedArray[1] = "";
        relatedArray[2] = "";

        auto temp = request.getParameterValues("related");

        for (auto i = temp.begin(); i != temp.end(); i++) {
            if ((*i) == "user")
                relatedArray[0] = "user";
            else if ((*i) == "forum")
                relatedArray[1] = "forum";
            else
                relatedArray[2] = "thread";
        }

        bool isPostExist = false;

        handleResponse();
        responseContent = PostInfo::getFullPostInfo(post_id, isPostExist);

        //  isUserExist костыль, надо как-то перерабыватывать функцию
        if (relatedArray[0] != "") {
            responseContent["user"] = UserInfo::getFullUserInfo(responseContent["user"].toString(), isPostExist);
        }
        if (relatedArray[1] != "") { //TODO
            // responseContent["user"] = UserInfo::getFullUserInfo(responseContent["user"], isUserExist);
        }
        if (responseContent["isDeleted"].toBool()) {
            responseContent["posts"] = 0;
        }
        objectResponce["response"] = responseContent;

        objectResponce["code"] = isPostExist ? 0 : 1;

        prepareOutput();
        response.setStatus(200);

        response.out() << output;
    }
};

class PostRemove : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~PostRemove()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {

        handlePostParams(request);
        handleResponse();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Posts SET isDeleted=true WHERE id=?;");
        query.bindValue(0, objectRequest["post"].toInt());
        bool ok = query.exec();
        responseContent["post"] = objectRequest["post"];

        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
    }
};

class PostRestore : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~PostRestore()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {

        handlePostParams(request);
        handleResponse();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Posts SET isDeleted=false WHERE id=?;");
        query.bindValue(0, objectRequest["post"].toInt());
        bool ok = query.exec();
        responseContent["post"] = objectRequest["post"];
        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
    }
};
#endif // POST_H
