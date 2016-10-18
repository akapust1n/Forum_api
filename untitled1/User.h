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

        /*esponseContent["username"] = objectRequest["username"];
        responseContent["about"] = objectRequest["about"];
        responseContent["name"] = objectRequest["name"].toString();
        responseContent["email"] = objectRequest["email"];
        responseContent["isAnonymous"] = objectRequest["isAnonymous"];*/
        bool tt= true;
        objectResponce["response"] = UserInfo::getFullUserInfo(objectRequest["email"].toString(), tt);

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
        bool test = QSqlDatabase::database("apidb1").transaction();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("INSERT INTO Followers (follower, followee) VALUES (:follower, :followee);");
        query.bindValue(":follower", objectRequest["follower"].toString());
        query.bindValue(":followee", objectRequest["followee"].toString());
        query.exec();
        bool ok = QSqlDatabase::database("apidb1").commit();

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

class UserUnFollow : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~UserUnFollow()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        handlePostParams(request);

        handleResponse();
        bool test = QSqlDatabase::database("apidb1").transaction();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("DELETE FROM Followers WHERE follower=:follower AND followee=:followee;");
        query.bindValue(":follower", objectRequest["follower"].toString());
        query.bindValue(":followee", objectRequest["followee"].toString());
        query.exec();
        bool ok = QSqlDatabase::database("apidb1").commit();

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
        bool test = QSqlDatabase::database("apidb1").transaction();

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Users SET about=:about, name=:name WHERE email=:user;");
        query.bindValue(":user", objectRequest["user"].toString());
        query.bindValue(":about", objectRequest["about"].toString());
        query.bindValue(":name", objectRequest["name"].toString());
        query.exec();
        bool ok = QSqlDatabase::database("apidb1").commit();

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
    {
        QString user;
        user = user.fromStdString(request.getParameter("user") ? *request.getParameter("user") : " ");

        QString order;
        order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : " ");
        //0 - magic constant for empty parametr
        QString since_id;
        since_id = user.fromStdString(request.getParameter("since_id") ? *request.getParameter("since_id") : " ");
        QString limit;
        limit = user.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : " ");

        QString str_since;
        QString str_limit;
        QString str_order;
        QString quote = "\"";

        if (since_id != " ")
            str_since = " AND id >= " + quote + since_id + quote;

        if (limit != " ")
            str_limit = " LIMIT " + limit;
        if (order == "asc")
            str_order = " ORDER BY name asc ";
        else
            str_order = " ORDER BY name desc";

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
        std::cout<<query.lastQuery().toStdString()<<"HEI3_"<<ok<<"__okis";
        objectResponce["response"] = arrayOfFollowers;

        prepareOutput();

        response.setStatus(200);

        response.out() << output;
    }
};

class UserListFollowing : public Wt::WResource, public HandleRequestList {
public:
    virtual ~UserListFollowing()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        QString user;
        user = user.fromStdString(request.getParameter("user") ? *request.getParameter("user") : " ");

        QString order;
        order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : " ");
        //0 - magic constant for empty parametr
        QString since_id;
        since_id = user.fromStdString(request.getParameter("since_id") ? *request.getParameter("since_id") : " ");
        QString limit;
        limit = user.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : " ");

        QString str_since;
        QString str_limit;
        QString str_order;
        QString quote = "\"";

        if (since_id != " ")
            str_since = " AND id >= " + quote + since_id + quote;

        if (limit != " ")
            str_limit = " LIMIT " + limit;
        if (order == "asc")
            str_order = " ORDER BY name asc";
        else
            str_order = " ORDER BY name desc";

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        QString expression;
        expression = "SELECT email FROM Users u JOIN Followers f ON  u.email = f.followee WHERE f.follower=" + quote + user + quote + str_since + str_order + str_limit + ";";
        bool ok = query.exec(expression);

        handleResponse();
        QJsonArray arrayOfFollowers;
        bool isUserExist = true; // заглушка
        if (ok) {
            while (query.next()) {
                QString email = query.value(0).toString();
                std::cout << "EMAIL_" << email.toStdString();
                QJsonObject jsonObj = UserInfo::getFullUserInfo(email, isUserExist); // assume this has been populated with Json data
                arrayOfFollowers << jsonObj;
                std::cout << "HERE22";
            }
        }
        objectResponce["response"] = arrayOfFollowers;
        std::cout << query.lastQuery().toStdString() << "hh2_" << arrayOfFollowers.isEmpty() << "_size";

        prepareOutput();

        response.setStatus(200);

        response.out() << output;
    }
};

class UserListPosts : public Wt::WResource, public HandleRequestList {
public:
    virtual ~UserListPosts()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        QString user;
        user = user.fromStdString(request.getParameter("user") ? *request.getParameter("user") : " ");

        QString order;
        order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : " ");
        //0 - magic constant for empty parametr
        QString since_id;
        since_id = user.fromStdString(request.getParameter("since") ? *request.getParameter("since") : " ");
        QString limit;
        limit = user.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : " ");

        QString str_since = " ";
        QString str_limit = " ";
        QString str_order = " ";
        QString quote = "\"";

        if (since_id != " ")
            str_since = " AND date >= " + quote + since_id + quote;
        if (limit != " ")
            str_limit = " LIMIT " + limit;
        if (order == "asc")
            str_limit = " ORDER by date asc ";
        else
            str_limit = " ORDER by date desc ";

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        QString expression;
        expression = "SELECT p.date, p.dislikes, p.forum, p.id, p.isApproved, p.isDeleted, p.isEdited, p.isHighlighted, p.isSpam, p.likes, p.message, p.thread_id, p.user, p.parent, p.likes-p.dislikes as points FROM Posts p WHERE p.user=" + quote + user + quote + str_since  + str_order + str_limit + ";";

        bool ok = query.exec(expression);
        std::cout << query.lastQuery().toStdString() << "HEI3";
        handleResponse();
        QJsonArray arrayOfPosts;
        bool isPostExist = true; // заглушка

        if (ok) {
            while (query.next()) {
                int id = query.value(3).toInt();
                QJsonObject jsonObj = PostInfo::getFullPostInfo(id, isPostExist); // assume this has been populated with Json data
                arrayOfPosts << jsonObj;
            }
        }
        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = arrayOfPosts;

        prepareOutput();

        response.setStatus(200);

        response.out() << output;
    }
};

#endif // USER_H
