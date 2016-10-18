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
#include <ThreadInfo.h>
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
        objectResponce["response"] = ForumInfo::getForumCreateInfo(objectRequest["short_name"].toString(), isForumExist);

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

class ForumListPosts : public Wt::WResource, public HandleRequestList {
public:
    virtual ~ForumListPosts()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        QString forum;
        forum = forum.fromStdString(request.getParameter("forum") ? *request.getParameter("forum") : "");

        QString order;
        order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : "");
        //0 - magic constant for empty parametr
        QString since_id;
        since_id = since_id.fromStdString(request.getParameter("since") ? *request.getParameter("since") : "");
        QString limit;
        limit = limit.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : "");

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

        QString str_since = " ";
        QString str_limit = " ";
        QString str_order = " ";
        QString quote = "\"";

        if (since_id != "")
            str_since = " AND p.date >= " + quote + since_id + quote;

        if (limit != "")
            str_limit = " LIMIT " + limit;
        if (order == "asc")
            str_order = " ORDER BY p.date asc";
        else
            str_order = " ORDER BY p.date  desc";

        // QString user;

        handleResponse();
        QJsonArray arrayOfPosts;
        QString expression = "SELECT id, user, message,forum,thread_id, parent, date,likes, dislikes,isApproved,isHighlighted,isEdited,isSpam,isDeleted FROM Posts p WHERE p.forum=" + quote + forum + quote + str_since + str_order + str_limit + ";";
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        bool ok = query.exec(expression);

        while (query.next()) {
            QString strGoodReply = Source::getPostTemplate();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
            QJsonObject jsonArray = jsonResponse.object();

            //   QJsonObject jsonArray;
            jsonArray["id"] = query.value(0).toInt();
            jsonArray["user"] = query.value(1).toString();
            jsonArray["message"] = query.value(2).toString();
            jsonArray["forum"] = query.value(3).toString();
            jsonArray["thread"] = query.value(4).toInt();

            if (!query.value(5).isNull())
                jsonArray["parent"] = query.value(5).toInt();
            else
                jsonArray["parent"] = QJsonValue::Null;

            jsonArray["date"] = query.value(6).toDateTime().toString("yyyy-MM-dd hh:mm:ss");

            jsonArray["likes"] = query.value(7).toInt();
            jsonArray["dislikes"] = query.value(8).toInt();
            jsonArray["isApproved"] = query.value(9).toBool();
            jsonArray["isHighlighted"] = query.value(10).toBool();
            jsonArray["isEdited"] = query.value(11).toBool();
            jsonArray["isSpam"] = query.value(12).toBool();
            jsonArray["isDeleted"] = query.value(13).toBool();
            jsonArray["points"] = query.value(7).toInt() - query.value(8).toInt();

            bool isUserExist = true; // костыль
            if (relatedArray[0] != "") {
                jsonArray["user"] = UserInfo::getFullUserInfo(jsonArray["user"].toString(), isUserExist);
            }
            if (relatedArray[1] != "") { //TODO
                jsonArray["forum"] = ForumInfo::getForumCreateInfo(forum, isUserExist);
            }
            if (relatedArray[2] != "") { //TODO
                jsonArray["thread"] = ThreadInfo::getFullThreadInfo(jsonArray["thread"].toInt(), isUserExist);
            }
            arrayOfPosts << jsonArray;

        }
       std::cout<<query.lastQuery().toStdString()<<"QUERY";
        objectResponce["response"] = arrayOfPosts;

        objectResponce["code"] = ok ? 0 : 1;

        prepareOutput();
        response.setStatus(200);

        response.out() << output;
    }
};
#endif // FORUM_H
