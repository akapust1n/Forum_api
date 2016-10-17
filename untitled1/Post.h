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

        if (objectRequest["parent"] != QJsonValue::Null)
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

class PostUpdate : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~PostUpdate()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {

        handlePostParams(request);
        bool isPostExist = false;

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Posts SET message=? WHERE id=?;");
        query.bindValue(0, objectRequest["message"].toString());
        query.bindValue(1, objectRequest["post"].toInt());
        bool ok = query.exec();
        handleResponse();
        responseContent = PostInfo::getFullPostInfo(objectRequest["post"].toInt(), isPostExist);
        responseContent["post"] = objectRequest["post"];
        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
    }
};

class PostVote : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~PostVote()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {

        handlePostParams(request);
        bool isPostExist = false;

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Posts SET likes=likes+?, dislikes=dislikes+? WHERE id =?;");
        query.bindValue(0, (objectRequest["vote"].toInt() > 0 ? 1 : 0));
        query.bindValue(1, (objectRequest["vote"].toInt() > 0 ? 0 : 1));
        query.bindValue(2, objectRequest["post"].toInt());
        bool ok = query.exec();
        handleResponse();
        responseContent = PostInfo::getFullPostInfo(objectRequest["post"].toInt(), isPostExist);
        responseContent["post"] = objectRequest["post"];
        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
    }
};

class PostList : public Wt::WResource, public HandleRequestList {
public:
    virtual ~PostList()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        QString threadOrForum;
        bool isForum = false;
        threadOrForum = threadOrForum.fromStdString(request.getParameter("thread") ? *request.getParameter("thread") : "");
        if (threadOrForum == "") {
            isForum = true;
            threadOrForum = threadOrForum.fromStdString(request.getParameter("forum") ? *request.getParameter("forum") : "");
        }
        QString order;
        order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : "desc");
        //0 - magic constant for empty parametr
        QString since_id;
        since_id = threadOrForum.fromStdString(request.getParameter("since") ? *request.getParameter("since") : "");
        QString limit;
        limit = threadOrForum.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : "");

        QString str_since;
        QString str_limit;
        QString str_order;
        QString quote = "\"";

        if (since_id != "")
            str_since = " AND p.date >= " + quote + since_id + quote;

        if (limit != "")
            str_limit = " LIMIT " + limit;
        if (order != "")
            str_order = " ORDER BY p.date " + order;

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        QString expression;
        if (!isForum) {
            expression = "SELECT p.id, p.date, p.dislikes, p.forum, p.dislikes, p.forum, p.id, p.isApproved, p.isDeleted, p.isEdited, p.isHighlighted, p.isSpam, p.likes, p.message, p.thread_id, p.user, p.parent, p.likes-p.dislikes as points FROM Posts p JOIN Threads second ON second.id=p.thread_id WHERE second.id=" + threadOrForum + str_since + str_order + str_limit + ";";

        } else {
            expression = "SELECT p.id,p.date, p.dislikes, p.forum, p.dislikes, p.forum, p.id, p.isApproved, p.isApproved, p.isDeleted, p.isEdited, p.isHighlighted, p.isSpam, p.likes, p.message, p.thread_id, p.user, p.parent, p.likes-p.dislikes as points FROM Posts p JOIN Forums second ON p.forum=second.short_name WHERE second.short_name=" + quote + threadOrForum + quote + str_since + str_order + str_limit + ";";
        }
        bool ok = query.exec(expression);

        handleResponse();
        QJsonArray arrayOfPosts;
        bool isUserExist = true; // заглушка

        if (ok) {
            while (query.next()) {
                int id = query.value(0).toInt();
                QJsonObject jsonObj = PostInfo::getFullPostInfo(id, isUserExist); // assume this has been populated with Json data
                arrayOfPosts << jsonObj;
            }
        }
        objectResponce["response"] = arrayOfPosts;

        prepareOutput();

        response.setStatus(200);

        response.out() << output;
    }
};

#endif // POST_H
