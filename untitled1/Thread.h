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
#include <QRegExp>
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
        if (ok) {
            objectResponce["code"] = 0;
            bool isThreadExist = true; //костыль(
            objectResponce["response"] = ThreadInfo::getThreadCreateInfo(query.lastInsertId().toInt(), isThreadExist);
        }

        else {
            objectResponce["code"] = 4;
        }

        prepareOutput();
        response.setStatus(200);

        response.out() << output;
        // std::cout << output << "Tut";
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
        if (responseContent["isDeleted"].toBool()) {
            responseContent["posts"] = 0;
        }
        objectResponce["response"] = responseContent;

        objectResponce["code"] = isThreadExist ? 0 : 1;

        prepareOutput();
        response.setStatus(200);

        response.out() << output;
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
        bool test = QSqlDatabase::database("apidb1").transaction();
        handlePostParams(request);
        handleResponse();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Threads SET isClosed=true WHERE id=?;");
        query.bindValue(0, objectRequest["thread"].toInt());
        query.exec();
        query.clear();
        bool ok = QSqlDatabase::database("apidb1").commit();

        responseContent["thread"] = objectRequest["thread"];
        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
    }
};
class ThreadOpen : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~ThreadOpen()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {

        handlePostParams(request);
        handleResponse();
        bool test = QSqlDatabase::database("apidb1").transaction();

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Threads SET isClosed=false WHERE id=?;");
        query.bindValue(0, objectRequest["thread"].toInt());
        query.exec();
        bool ok = QSqlDatabase::database("apidb1").commit();

        responseContent["thread"] = objectRequest["thread"];
        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
    }
};

class ThreadRemove : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~ThreadRemove()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        handlePostParams(request);
        handleResponse();
        bool test = QSqlDatabase::database("apidb1").transaction();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Threads SET isDeleted=true WHERE id=?;");
        query.bindValue(0, objectRequest["thread"].toInt());
        query.exec();

        QSqlQuery query2(QSqlDatabase::database("apidb1"));
        query2.prepare("UPDATE Posts SET isDeleted=true WHERE thread_id=?;");
        query2.bindValue(0, objectRequest["thread"].toInt());
        query2.exec();
        bool ok = QSqlDatabase::database("apidb1").commit();

        responseContent["thread"] = objectRequest["thread"];

        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
    }
};

class ThreadRestore : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~ThreadRestore()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {

        handlePostParams(request);
        handleResponse();
        bool test = QSqlDatabase::database("apidb1").transaction();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Threads SET isDeleted=false WHERE id=?;");
        query.bindValue(0, objectRequest["thread"].toInt());
        query.exec();

        QSqlQuery query2(QSqlDatabase::database("apidb1"));
        query2.prepare("UPDATE Posts SET isDeleted=false WHERE thread_id=?;");
        query2.bindValue(0, objectRequest["thread"].toInt());
        query2.exec();
        bool ok = QSqlDatabase::database("apidb1").commit();
        std::cout << "Restore___" << ok << "__id__" << objectRequest["thread"].toInt();

        responseContent["thread"] = objectRequest["thread"];
        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
    }
};

class ThreadUpdate : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~ThreadUpdate()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        handlePostParams(request);
        handleResponse();
        bool test = QSqlDatabase::database("apidb1").transaction();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Threads SET message=?, slug=? WHERE id=?;");
        query.bindValue(0, objectRequest["message"].toString());
        query.bindValue(1, objectRequest["slug"].toString());
        query.bindValue(2, objectRequest["thread"].toInt());

        query.exec();

        bool ok = QSqlDatabase::database("apidb1").commit();

        responseContent["thread"] = objectRequest["thread"];

        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
    }
};

class ThreadVote : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~ThreadVote()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        handlePostParams(request);
        handleResponse();
        bool test = QSqlDatabase::database("apidb1").transaction();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("UPDATE Threads SET likes=likes+?, dislikes=dislikes+? WHERE id =?;");
        query.bindValue(0, objectRequest["vote"].toInt() > 0);
        query.bindValue(1, objectRequest["vote"].toInt() < 0);
        query.bindValue(2, objectRequest["thread"].toInt());

        query.exec();

        bool ok = QSqlDatabase::database("apidb1").commit();

        responseContent["thread"] = objectRequest["thread"];

        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
    }
};

class ThreadSubscribe : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~ThreadSubscribe()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        handlePostParams(request);
        handleResponse();
        bool test = QSqlDatabase::database("apidb1").transaction();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("INSERT INTO Subscribers (user, thread_id) VALUES (?, ?);");
        query.bindValue(0, objectRequest["user"].toString());
        query.bindValue(1, objectRequest["thread"].toInt());

        query.exec();

        bool ok = QSqlDatabase::database("apidb1").commit();

        responseContent["thread"] = objectRequest["thread"];

        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
    }
};

class ThreadUnSubscribe : public Wt::WResource, public HandleRequestBase {
public:
    virtual ~ThreadUnSubscribe()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        handlePostParams(request);
        handleResponse();
        bool test = QSqlDatabase::database("apidb1").transaction();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("DELETE FROM Subscribers WHERE user=? AND thread=?;");
        query.bindValue(0, objectRequest["user"].toString());
        query.bindValue(1, objectRequest["thread"].toInt());

        query.exec();

        bool ok = QSqlDatabase::database("apidb1").commit();

        responseContent["thread"] = objectRequest["thread"];

        objectResponce["code"] = ok ? 0 : 1;
        objectResponce["response"] = responseContent;

        prepareOutput();
        response.setStatus(200);
        response.out() << output;
    }
};
class ThreadList : public Wt::WResource, public HandleRequestList {
public:
    virtual ~ThreadList()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        QString userOrForum;
        bool isForum = false;
        userOrForum = userOrForum.fromStdString(request.getParameter("user") ? *request.getParameter("user") : "");
        if (userOrForum == "") {
            isForum = true;
            userOrForum = userOrForum.fromStdString(request.getParameter("forum") ? *request.getParameter("forum") : "");
        }
        QString order;
        order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : "desc");
        //0 - magic constant for empty parametr
        QString since_id;
        since_id = userOrForum.fromStdString(request.getParameter("since") ? *request.getParameter("since") : "");
        QString limit;
        limit = userOrForum.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : "");

        QString str_since;
        QString str_limit;
        QString str_order;
        QString quote = "\"";

        if (since_id != "")
            str_since = " AND date >= " + quote + since_id + quote;

        if (limit != "")
            str_limit = " LIMIT " + limit;
        if (order != "")
            str_order = " ORDER BY p.date " + order;

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        QString expression;
        if (!isForum) {
            expression = "SELECT * FROM Threads WHERE user=" + quote + userOrForum+ quote + str_since + str_order + str_limit + ";";

        } else {
            expression = "SELECT * FROM Threads WHERE forum=" + quote + userOrForum + quote + str_since + str_order + str_limit + ";";
        }
        bool ok = query.exec(expression);

        handleResponse();
        QJsonArray arrayOfThreads;
        bool isThreadExist = true; // заглушка

        if (ok) {
            while (query.next()) {
                int id = query.value(0).toInt();
                QJsonObject jsonObj = ThreadInfo::getFullThreadInfo(id, isThreadExist); // assume this has been populated with Json data
                arrayOfThreads << jsonObj;
            }
        }
        objectResponce["response"] = arrayOfThreads;

        prepareOutput();

        response.setStatus(200);

        response.out() << output;
    }
};

class ThreadListPost : public Wt::WResource, public HandleRequestList {
public:
    virtual ~ThreadListPost()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        QString thread;
        thread = thread.fromStdString(request.getParameter("thread") ? *request.getParameter("thread") : "");

        QString order;
        order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : "");
        //0 - magic constant for empty parametr
        QString since_id;
        since_id = thread.fromStdString(request.getParameter("since") ? *request.getParameter("since") : "");
        QString limit;
        limit = thread.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : "");
        QString sort;
        sort = thread.fromStdString(request.getParameter("sort") ? *request.getParameter("sort") : "");

        QString str_since = " ";
        QString str_limit = " ";
        QString str_order = " ";
        QString str_sort = " ";
        QString quote = "\"";

        if (since_id != "")
            str_since = " AND date >= " + quote + since_id + quote;
        if (limit != "")
            str_limit = " LIMIT " + limit;

        if (sort == "flat" || sort == "")
            str_sort = " ORDER BY date ";
        //if (!order.isEmpty())
        str_order = " " + order;
        if (sort == "tree") {
            str_sort = "ORDER BY  LPAD(path, 1, \"0\") desc, path asc ";
            if (order == "asc") {
                str_sort = " ORDER BY path asc";
            }
            str_order = " ";
        }
        if (sort == "parent_tree") {
            if (order == "asc") {
                QSqlQuery query2(QSqlDatabase::database("apidb1"));
                QString expression = "SELECT Min(path) FROM Posts p WHERE p.thread_id=" + quote + thread + quote + str_since + ";";
                query2.exec(expression);
                query2.next();
                QString max_value = query2.value(0).toString();
                int max_val = max_value.toInt(0, BASE);
                QString tmp = QString::number(max_val + limit.toInt() - 1, BASE);

                tmp = tmp + "z" + "z" + "z" + "z" + "z";
                str_sort = " AND (path <" + quote + tmp + quote + ")";
                str_sort += " ORDER BY path ASC";
                str_limit = " ";
            }

            if (order == "desc" || order == " ") {
                //очень грустный момент
                QSqlQuery query2(QSqlDatabase::database("apidb1"));
                QString expression = "SELECT Count(path) FROM Posts p WHERE p.thread_id=" + quote + thread + quote + str_since + ";";
                query2.exec(expression);
                query2.next();
                int max_value = query2.value(0).toInt();
                QString tmp = QString::number(max_value - limit.toInt() - 1, BASE);

                tmp = tmp + "0" + "0" + "0" + "0" + "0";
                str_sort = " AND (path >" + quote + tmp + quote + ")";
                str_sort += " order by path desc";
                str_limit = " ";
            }
            str_order = " ";
        }
        // str_order = "";

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        QString expression;
        expression = "SELECT p.date, p.dislikes, p.forum, p.id, p.isApproved, p.isDeleted, p.isEdited, p.isHighlighted, p.isSpam, p.likes, p.message, p.thread_id, p.user, p.parent, p.likes-p.dislikes as points FROM Posts p WHERE p.thread_id=" + quote + thread + quote + str_since + str_sort + str_order + str_limit + ";";

        bool ok = query.exec(expression);
        std::cout << query.lastQuery().toStdString() << "HEI2";
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
#endif // THREAD_H
