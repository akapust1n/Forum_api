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


class UserCreate : public Wt::WResource {
public:
    virtual ~UserCreate()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
HandleRequestBase hR;
        hR.handlePostParams(request);
        QString conName = BdWrapper::getConnection();

        bool test = QSqlDatabase::database(conName).transaction();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("INSERT INTO Users (username, about, name, email, isAnonymous) VALUES (:username, :about, :name, :email, :isAnonymous);");
        query.bindValue(":username", hR.objectRequest["username"].toString());
        query.bindValue(":about", hR.objectRequest["about"].toString());
        query.bindValue(":name", hR.objectRequest["name"].toString());
        query.bindValue(":email", hR.objectRequest["email"].toString());
        query.bindValue(":isAnonymous", hR.objectRequest["isAnonymous"].toBool());
         bool ok =query.exec();
         QSqlDatabase::database(conName).commit();


        hR.handleResponse();
        hR.objectResponce["code"] = QJsonValue(ok ? 0 : 5);

        /*esponseContent["username"] = hR.objectRequest["username"];
        hR.responseContent["about"] = hR.objectRequest["about"];
        hR.responseContent["name"] = hR.objectRequest["name"].toString();
        hR.responseContent["email"] = hR.objectRequest["email"];
        hR.responseContent["isAnonymous"] = hR.objectRequest["isAnonymous"];*/
        bool tt= true;
        hR.objectResponce["response"] = UserInfo::getFullUserInfo(hR.objectRequest["email"].toString(), tt);

        hR.prepareOutput();
        response.setStatus(200);

        response.out() << hR.output;
        //std::cout << hR.output << "Tut";
        BdWrapper::closeConnection(conName);

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
    {HandleRequestBase hR;
        //ПЕРЕПИСАТЬ В USERINFO ВСЕ ЗАПРОСЫ ПРО ЮЗЕРА ((
        //на данный момент это просто захардкожено для прикола
        //Возможно, уже переписал
        QString user;
        user = user.fromStdString(request.getParameter("user") ? *request.getParameter("user") : "");

        QString strGoodReply = Source::getAnswerTemplate();
        bool isUserExist = false;

        hR.handleResponse();
        hR.responseContent = UserInfo::getFullUserInfo(user, isUserExist);
        hR.objectResponce["response"] = hR.responseContent;

        hR.objectResponce["code"] = isUserExist ? 0 : 1;

        hR.prepareOutput();
        response.setStatus(200);
        response.out() << hR.output;
        // std::cout << data.toStdString() << std::endl;
    }
};

class UserFollow : public Wt::WResource{
public:
    virtual ~UserFollow()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {HandleRequestBase hR;
        hR.handlePostParams(request);

        hR.handleResponse();
        QString conName = BdWrapper::getConnection();

        bool test = QSqlDatabase::database(conName).transaction();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("INSERT INTO Followers (follower, followee) VALUES (:follower, :followee);");
        query.bindValue(":follower", hR.objectRequest["follower"].toString());
        query.bindValue(":followee", hR.objectRequest["followee"].toString());
        query.exec();
        bool ok = QSqlDatabase::database(conName).commit();

        bool isUserExist = false;
        QJsonObject responseContent = UserInfo::getFullUserInfo(hR.objectRequest["follower"].toString(), isUserExist);
        if (isUserExist) {
            hR.objectResponce["code"] = 0;
            hR.objectResponce["response"] = hR.responseContent;
        } else {
            hR.objectResponce["code"] = 1;
            hR.objectResponce["response"] = "error message";
        }

        hR.prepareOutput();
        response.setStatus(200);

        response.out() << hR.output;
        BdWrapper::closeConnection(conName);

    }
};

class UserUnFollow : public Wt::WResource {
public:
    virtual ~UserUnFollow()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {HandleRequestBase hR;
        hR.handlePostParams(request);

        hR.handleResponse();
        QString conName = BdWrapper::getConnection();

        bool test = QSqlDatabase::database(conName).transaction();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("DELETE FROM Followers WHERE follower=:follower AND followee=:followee;");
        query.bindValue(":follower", hR.objectRequest["follower"].toString());
        query.bindValue(":followee", hR.objectRequest["followee"].toString());
        query.exec();
        bool ok = QSqlDatabase::database(conName).commit();

        bool isUserExist = false;
        QJsonObject responseContent = UserInfo::getFullUserInfo(hR.objectRequest["follower"].toString(), isUserExist);
        if (isUserExist) {
            hR.objectResponce["code"] = 0;
            hR.objectResponce["response"] = hR.responseContent;
        } else {
            hR.objectResponce["code"] = 1;
            hR.objectResponce["response"] = "error message";
        }

        hR.prepareOutput();
        response.setStatus(200);

        response.out() << hR.output;
        BdWrapper::closeConnection(conName);

    }
};
class UserUpdateProfile : public Wt::WResource{
public:
    virtual ~UserUpdateProfile()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {HandleRequestBase hR;
        hR.handlePostParams(request);

        hR.handleResponse();
        QString conName = BdWrapper::getConnection();

        bool test = QSqlDatabase::database(conName).transaction();

        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("UPDATE Users SET about=:about, name=:name WHERE email=:user;");
        query.bindValue(":user", hR.objectRequest["user"].toString());
        query.bindValue(":about", hR.objectRequest["about"].toString());
        query.bindValue(":name", hR.objectRequest["name"].toString());
        query.exec();
        bool ok = QSqlDatabase::database(conName).commit();

        bool isUserExist = true;
        QJsonObject responseContent = UserInfo::getFullUserInfo(hR.objectRequest["user"].toString(), isUserExist);
        if (isUserExist) {
            hR.objectResponce["code"] = 0;
            hR.objectResponce["response"] = hR.responseContent;
        } else {
            hR.objectResponce["code"] = 1;
            hR.objectResponce["response"] = "error message";
        }

        hR.prepareOutput();

        response.out() << hR.output;
        // std::cout << "DATA____" << data.toStdString();
        BdWrapper::closeConnection(conName);

    }
};

class UserListFollowers : public Wt::WResource {
public:
    virtual ~UserListFollowers()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestList  hR;
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
        QString conName = BdWrapper::getConnection();
        QSqlQuery query(QSqlDatabase::database(conName));
        QString expression;
        expression = "SELECT email FROM Users u JOIN Followers f ON  u.email = f.follower WHERE f.followee=" + quote + user + quote + str_since + str_order + str_limit + ";";
        bool ok = query.exec(expression);


        hR.handleResponse();
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
        hR.objectResponce["response"] = arrayOfFollowers;

        hR.prepareOutput();

        response.setStatus(200);

        response.out() << hR.output;
        BdWrapper::closeConnection(conName);

    }
};

class UserListFollowing : public Wt::WResource {
public:
    virtual ~UserListFollowing()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestList  hR;
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
        QString conName = BdWrapper::getConnection();
        QSqlQuery query(QSqlDatabase::database(conName));
        QString expression;
        expression = "SELECT email FROM Users u JOIN Followers f ON  u.email = f.followee WHERE f.follower=" + quote + user + quote + str_since + str_order + str_limit + ";";
        bool ok = query.exec(expression);

        hR.handleResponse();
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
        hR.objectResponce["response"] = arrayOfFollowers;
        std::cout << query.lastQuery().toStdString() << "hh2_" << arrayOfFollowers.isEmpty() << "_size";

        hR.prepareOutput();

        response.setStatus(200);

        response.out() << hR.output;
        BdWrapper::closeConnection(conName);

    }
};

class UserListPosts : public Wt::WResource {
public:
    virtual ~UserListPosts()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestList  hR;
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
            str_order = " ORDER by date asc ";
        else
            str_order = " ORDER by date desc ";
        QString conName = BdWrapper::getConnection();

        QSqlQuery query(QSqlDatabase::database(conName));
        QString expression;
        expression = "SELECT p.id FROM Posts p WHERE p.user=" + quote + user + quote + str_since  + str_order + str_limit + ";";

        bool ok = query.exec(expression);
        std::cout << query.lastQuery().toStdString() << "HEI3";
        hR.handleResponse();
        QJsonArray arrayOfPosts;
        bool isPostExist = true; // заглушка

        if (ok) {
            while (query.next()) {
                int id = query.value(0).toInt();
                QJsonObject jsonObj = PostInfo::getFullPostInfo(id, isPostExist); // assume this has been populated with Json data
                arrayOfPosts << jsonObj;
            }
        }
        hR.objectResponce["code"] = ok ? 0 : 1;
        hR.objectResponce["response"] = arrayOfPosts;

        hR.prepareOutput();

        response.setStatus(200);

        response.out() << hR.output;
        BdWrapper::closeConnection(conName);

    }
};

#endif // USER_H

