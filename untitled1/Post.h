#ifndef POST_H
#define POST_H
#include "ForumInfo.h"
#include "PostInfo.h"
#include "ThreadInfo.h"
#include "ThreadInfo.h"
#include "Trash.h"
#include "User.h"
#include <BdWrapper.h>
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

class PostCreate : public Wt::WResource {
public:
    virtual ~PostCreate()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestBase hR;
        hR.handlePostParams(request);
        if (hR.objectRequest["isDeleted"] == "")
            hR.objectRequest["isDeleted"] = false;

        if (hR.objectRequest["parent"] == "")
            hR.objectRequest["parent"] = false;

        if (hR.objectRequest["isApproved"] == "")
            hR.objectRequest["isApproved"] = false;

        if (hR.objectRequest["isHighlighted"] == "")
            hR.objectRequest["isHighlighted"] = false;

        if (hR.objectRequest["isEdited"] == "")
            hR.objectRequest["isEdited"] = true;

        if (hR.objectRequest["isSpam"] == "")
            hR.objectRequest["isSpam"] = false;
        QString path;
        if (hR.objectRequest["parent"] == QJsonValue::Null)
            path = "";
        QString conName = BdWrapper::getConnection();
        bool test = QSqlDatabase::database(conName).transaction();

        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("INSERT INTO Posts (date, thread_id, message, user, forum, parent, isApproved, isHighlighted, isEdited, isSpam, isDeleted, path) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
        query.bindValue(0, hR.objectRequest["date"].toString());
        query.bindValue(1, hR.objectRequest["thread"].toInt());
        query.bindValue(2, hR.objectRequest["message"].toString());
        query.bindValue(3, hR.objectRequest["user"].toString());

        // if (hR.objectRequest["forum"] != QJsonValue::Null)
        query.bindValue(4, hR.objectRequest["forum"].toString());
        //  else
        //     query.bindValue(4, QJsonValue::Null);

        if (hR.objectRequest["parent"] != QJsonValue::Null) {
            query.bindValue(5, hR.objectRequest["parent"].toInt());
            path = PostInfo::getPath(hR.objectRequest["parent"].toInt());

        } else {
            path = PostInfo::getPath(-1);
            query.bindValue(5, QVariant::Int);
        }
        query.bindValue(6, hR.objectRequest["isApproved"].toBool());
        query.bindValue(7, hR.objectRequest["isHighlighted"].toBool());
        query.bindValue(8, hR.objectRequest["isEdited"].toBool());
        query.bindValue(9, hR.objectRequest["isSpam"].toBool());
        query.bindValue(10, hR.objectRequest["isDeleted"].toBool());

        query.bindValue(11, path);
        bool ok = query.exec();
        bool o2 = QSqlDatabase::database(conName).commit();

        auto tt = query.lastError().text();

        hR.handleResponse();
        if (ok) {

            //обновляем path. ТУТ ДОЛЖНА БЫТЬ ТРАНЗАКЦИЯ(наверное)
            //            int last_id = query.lastInsertId().toInt();
            //            if (path==""){
            //            QString str_last_id = QString::number(last_id-1, BASE);
            //            QSqlQuery query2(QSqlDatabase::database(conName)));
            //            query2.prepare("UPDATE Posts SET path=? WHERE id=?;");
            //            query2.bindValue(0, str_last_id);
            //            query2.bindValue(1, last_id);
            //           bool ok2= query2.exec();
            //           std::cout<<str_last_id.toStdString()<<"___"<<ok2<<"HAIl";}

            hR.objectResponce["code"] = 0;
            bool isThreadExist = true; //костыль(

            hR.objectResponce["response"] = PostInfo::getFullPostInfo(query.lastInsertId().toInt(), isThreadExist);
        }

        else {
            hR.objectResponce["code"] = 4;
        }

        hR.prepareOutput();
        response.setStatus(200);

        response.out() << hR.output;
        BdWrapper::closeConnection(conName);

        // std::cout << hR.output << "Tat";
    }
};

class PostDetails : public Wt::WResource {
public:
    virtual ~PostDetails()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestBase hR;
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

        hR.handleResponse();
        hR.responseContent = PostInfo::getFullPostInfo(post_id, isPostExist);

        //  isUserExist костыль, надо как-то перерабыватывать функцию
        if (relatedArray[0] != "") {
            hR.responseContent["user"] = UserInfo::getFullUserInfo(hR.responseContent["user"].toString(), isPostExist);
        }
        if (relatedArray[1] != "") { //TODO
            hR.responseContent["forum"] = ForumInfo::getFullForumInfo(hR.responseContent["forum"].toString(), isPostExist);
        }
        if (relatedArray[2] != "") { //TODO
            hR.responseContent["thread"] = ThreadInfo::getFullThreadInfo(hR.responseContent["thread"].toInt(), isPostExist);
        }

        hR.objectResponce["response"] = hR.responseContent;

        hR.objectResponce["code"] = isPostExist ? 0 : 1;

        hR.prepareOutput();
        response.setStatus(200);

        response.out() << hR.output;
    }
};

class PostRemove : public Wt::WResource {
public:
    virtual ~PostRemove()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestBase hR;
        hR.handlePostParams(request);
        hR.handleResponse();
        QString conName = BdWrapper::getConnection();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("UPDATE Posts SET isDeleted=true WHERE id=?;");
        query.bindValue(0, hR.objectRequest["post"].toInt());
        bool ok = query.exec();
        hR.responseContent["post"] = hR.objectRequest["post"];

        hR.objectResponce["code"] = ok ? 0 : 1;
        hR.objectResponce["response"] = hR.responseContent;

        hR.prepareOutput();
        response.setStatus(200);
        response.out() << hR.output;
        BdWrapper::closeConnection(conName);
    }
};

class PostRestore : public Wt::WResource {
public:
    virtual ~PostRestore()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestBase hR;
        hR.handlePostParams(request);
        hR.handleResponse();
        QString conName = BdWrapper::getConnection();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("UPDATE Posts SET isDeleted=false WHERE id=?;");
        query.bindValue(0, hR.objectRequest["post"].toInt());
        bool ok = query.exec();
        hR.responseContent["post"] = hR.objectRequest["post"];
        hR.objectResponce["code"] = ok ? 0 : 1;
        hR.objectResponce["response"] = hR.responseContent;

        hR.prepareOutput();
        response.setStatus(200);
        response.out() << hR.output;
        BdWrapper::closeConnection(conName);
    }
};

class PostUpdate : public Wt::WResource {
public:
    virtual ~PostUpdate()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestBase hR;
        hR.handlePostParams(request);
        bool isPostExist = false;
        QString conName = BdWrapper::getConnection();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("UPDATE Posts SET message=? WHERE id=?;");
        query.bindValue(0, hR.objectRequest["message"].toString());
        query.bindValue(1, hR.objectRequest["post"].toInt());
        bool ok = query.exec();
        hR.handleResponse();
        hR.responseContent = PostInfo::getFullPostInfo(hR.objectRequest["post"].toInt(), isPostExist);
        hR.responseContent["post"] = hR.objectRequest["post"];
        hR.objectResponce["code"] = ok ? 0 : 1;
        hR.objectResponce["response"] = hR.responseContent;

        hR.prepareOutput();
        response.setStatus(200);
        response.out() << hR.output;
        BdWrapper::closeConnection(conName);
    }
};

class PostVote : public Wt::WResource{
public:
    virtual ~PostVote()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestBase hR;
        hR.handlePostParams(request);
        bool isPostExist = false;
        QString conName = BdWrapper::getConnection();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("UPDATE Posts SET likes=likes+?, dislikes=dislikes+? WHERE id =?;");
        query.bindValue(0, (hR.objectRequest["vote"].toInt() > 0 ? 1 : 0));
        query.bindValue(1, (hR.objectRequest["vote"].toInt() > 0 ? 0 : 1));
        query.bindValue(2, hR.objectRequest["post"].toInt());
        bool ok = query.exec();
        hR.handleResponse();
        hR.responseContent = PostInfo::getFullPostInfo(hR.objectRequest["post"].toInt(), isPostExist);
        hR.responseContent["post"] = hR.objectRequest["post"];
        hR.objectResponce["code"] = ok ? 0 : 1;
        hR.objectResponce["response"] = hR.responseContent;

        hR.prepareOutput();
        response.setStatus(200);
        response.out() << hR.output;
        BdWrapper::closeConnection(conName);
    }
};

class PostList : public Wt::WResource {
public:
    virtual ~PostList()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestList hR;
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
        QString conName = BdWrapper::getConnection();
        QSqlQuery query(QSqlDatabase::database(conName));
        QString expression;
        if (!isForum) {
            expression = "SELECT p.id, p.date, p.dislikes, p.forum, p.dislikes, p.forum, p.id, p.isApproved, p.isDeleted, p.isEdited, p.isHighlighted, p.isSpam, p.likes, p.message, p.thread_id, p.user, p.parent, p.likes-p.dislikes as points FROM Posts p JOIN Threads second ON second.id=p.thread_id WHERE second.id=" + threadOrForum + str_since + str_order + str_limit + ";";

        } else {
            expression = "SELECT p.id,p.date, p.dislikes, p.forum, p.dislikes, p.forum, p.id, p.isApproved, p.isApproved, p.isDeleted, p.isEdited, p.isHighlighted, p.isSpam, p.likes, p.message, p.thread_id, p.user, p.parent, p.likes-p.dislikes as points FROM Posts p JOIN Forums second ON p.forum=second.short_name WHERE second.short_name=" + quote + threadOrForum + quote + str_since + str_order + str_limit + ";";
        }
        bool ok = query.exec(expression);

        hR.handleResponse();
        QJsonArray arrayOfPosts;
        bool isUserExist = true; // заглушка

        if (ok) {
            while (query.next()) {
                int id = query.value(0).toInt();
                QJsonObject jsonObj = PostInfo::getFullPostInfo(id, isUserExist); // assume this has been populated with Json data
                arrayOfPosts << jsonObj;
            }
        }
        hR.objectResponce["response"] = arrayOfPosts;

        hR.prepareOutput();

        response.setStatus(200);

        response.out() << hR.output;
        BdWrapper::closeConnection(conName);
    }
};

#endif // POST_H
