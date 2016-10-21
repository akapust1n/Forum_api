#ifndef THREAD_H
#define THREAD_H
#include "ThreadInfo.h"
#include "Trash.h"
#include "User.h"
#include <BdWrapper.h>
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

class ThreadCreate : public Wt::WResource {
public:
    virtual ~ThreadCreate()
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
        QString conName = BdWrapper::getConnection();
        bool test = QSqlDatabase::database(conName).transaction();

        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("INSERT INTO Threads (forum, title, isClosed, user, date, message, slug, isDeleted) VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
        query.bindValue(0, hR.objectRequest["forum"].toString());
        query.bindValue(1, hR.objectRequest["title"].toString());
        query.bindValue(2, hR.objectRequest["isClosed"].toBool());
        query.bindValue(3, hR.objectRequest["user"].toString());
        query.bindValue(4, hR.objectRequest["date"].toString());
        query.bindValue(5, hR.objectRequest["message"].toString());
        query.bindValue(6, hR.objectRequest["slug"].toString());
        query.bindValue(7, hR.objectRequest["isDeleted"].toBool());
        query.exec();
        bool ok = QSqlDatabase::database(conName).commit();

        hR.handleResponse();
        if (ok) {
            hR.objectResponce["code"] = 0;
            bool isThreadExist = true; //костыль(
            hR.objectResponce["response"] = ThreadInfo::getThreadCreateInfo(query.lastInsertId().toInt(), isThreadExist);
        }

        else {
            hR.objectResponce["code"] = 4;
        }

        hR.prepareOutput();
        response.setStatus(200);

        response.out() << hR.output;
        // std::cout << hR.output << "Tut";
    }
};
class ThreadDetails : public Wt::WResource {
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
        HandleRequestBase hR;
        QString thread;
        thread = thread.fromStdString(request.getParameter("thread") ? *request.getParameter("thread") : "");
        int thread_id = thread.toInt();
        QString relatedArray[2]; //0-user, 1-forum
        relatedArray[0] = "";
        relatedArray[1] = "";
        bool error = false;

        auto temp = request.getParameterValues("related");

        for (auto i = temp.begin(); i != temp.end(); i++) {
            if ((*i) == "user")
                relatedArray[0] = "user";
            else if ((*i) == "forum")
                relatedArray[1] = "forum";
            else
                error = true;
        }

        // QString user;
        // user= thread.fromStdString(request.getParameter("thread") ? *request.getParameter("thread") : "");

        bool isThreadExist = false;

        hR.handleResponse();
        hR.responseContent = ThreadInfo::getFullThreadInfo(thread_id, isThreadExist);

        //  isUserExist костыль, надо как-то перерабыватывать функцию
        if (relatedArray[0] != "") {
            hR.responseContent["user"] = UserInfo::getFullUserInfo(hR.responseContent["user"].toString(), isThreadExist);
        }
        if (relatedArray[1] != "") { //TODO
            // hR.responseContent["user"] = UserInfo::getFullUserInfo(hR.responseContent["user"], isUserExist);
        }
        if (hR.responseContent["isDeleted"].toBool()) {
            hR.responseContent["posts"] = 0;
        }
        hR.objectResponce["response"] = hR.responseContent;

        hR.objectResponce["code"] = isThreadExist ? 0 : 1;
        if (error)
            hR.objectResponce["code"] = 3;

        hR.prepareOutput();
        response.setStatus(200);

        response.out() << hR.output;
    }
};
class ThreadClose : public Wt::WResource {
public:
    virtual ~ThreadClose()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestBase hR;
        QString conName = BdWrapper::getConnection();
        bool test = QSqlDatabase::database(conName).transaction();
        hR.handlePostParams(request);
        hR.handleResponse();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("UPDATE Threads SET isClosed=true WHERE id=?;");
        query.bindValue(0, hR.objectRequest["thread"].toInt());
        query.exec();
        query.clear();
        bool ok = QSqlDatabase::database(conName).commit();

        hR.responseContent["thread"] = hR.objectRequest["thread"];
        hR.objectResponce["code"] = ok ? 0 : 1;
        hR.objectResponce["response"] = hR.responseContent;

        hR.prepareOutput();
        response.setStatus(200);
        response.out() << hR.output;
        BdWrapper::closeConnection(conName);
    }
};
class ThreadOpen : public Wt::WResource {
public:
    virtual ~ThreadOpen()
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
        bool test = QSqlDatabase::database(conName).transaction();

        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("UPDATE Threads SET isClosed=false WHERE id=?;");
        query.bindValue(0, hR.objectRequest["thread"].toInt());
        query.exec();
        bool ok = QSqlDatabase::database(conName).commit();

        hR.responseContent["thread"] = hR.objectRequest["thread"];
        hR.objectResponce["code"] = ok ? 0 : 1;
        hR.objectResponce["response"] = hR.responseContent;

        hR.prepareOutput();
        response.setStatus(200);
        response.out() << hR.output;
    }
};

class ThreadRemove : public Wt::WResource {
public:
    virtual ~ThreadRemove()
    {
        beingDeleted();
    };

protected:
    //тут должен быть роллбек
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestBase hR;
        hR.handlePostParams(request);
        hR.handleResponse();
        QString conName = BdWrapper::getConnection();
        bool test = QSqlDatabase::database(conName).transaction();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("UPDATE Threads SET isDeleted=true WHERE id=?;");
        query.bindValue(0, hR.objectRequest["thread"].toInt());
        query.exec();

        QSqlQuery query2(QSqlDatabase::database(conName));
        query2.prepare("UPDATE Posts SET isDeleted=true WHERE thread_id=?;");
        query2.bindValue(0, hR.objectRequest["thread"].toInt());
        query2.exec();
        bool ok = QSqlDatabase::database(conName).commit();

        hR.responseContent["thread"] = hR.objectRequest["thread"];

        hR.objectResponce["code"] = ok ? 0 : 1;
        hR.objectResponce["response"] = hR.responseContent;

        hR.prepareOutput();
        response.setStatus(200);
        response.out() << hR.output;
        BdWrapper::closeConnection(conName);
    }
};

class ThreadRestore : public Wt::WResource {
public:
    virtual ~ThreadRestore()
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
        bool test = QSqlDatabase::database(conName).transaction();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("UPDATE Threads SET isDeleted=false WHERE id=?;");
        query.bindValue(0, hR.objectRequest["thread"].toInt());
        query.exec();

        QSqlQuery query2(QSqlDatabase::database(conName));
        query2.prepare("UPDATE Posts SET isDeleted=false WHERE thread_id=?;");
        query2.bindValue(0, hR.objectRequest["thread"].toInt());
        query2.exec();
        bool ok = QSqlDatabase::database(conName).commit();
        std::cout << "Restore___" << ok << "__id__" << hR.objectRequest["thread"].toInt();

        hR.responseContent["thread"] = hR.objectRequest["thread"];
        hR.objectResponce["code"] = ok ? 0 : 1;
        hR.objectResponce["response"] = hR.responseContent;

        hR.prepareOutput();
        response.setStatus(200);
        response.out() << hR.output;
        BdWrapper::closeConnection(conName);
    }
};

class ThreadUpdate : public Wt::WResource {
public:
    virtual ~ThreadUpdate()
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
        bool test = QSqlDatabase::database(conName).transaction();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("UPDATE Threads SET message=?, slug=? WHERE id=?;");
        query.bindValue(0, hR.objectRequest["message"].toString());
        query.bindValue(1, hR.objectRequest["slug"].toString());
        query.bindValue(2, hR.objectRequest["thread"].toInt());

        query.exec();

        bool ok = QSqlDatabase::database(conName).commit();

        hR.objectResponce["code"] = ok ? 0 : 1;
        bool tt = true;
        hR.objectResponce["response"] = ThreadInfo::getFullThreadInfo(hR.objectRequest["thread"].toInt(), tt);

        hR.prepareOutput();
        response.setStatus(200);
        response.out() << hR.output;
        BdWrapper::closeConnection(conName);
    }
};

class ThreadVote : public Wt::WResource {
public:
    virtual ~ThreadVote()
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
        bool test = QSqlDatabase::database(conName).transaction();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("UPDATE Threads SET likes=likes+?, dislikes=dislikes+? WHERE id =?;");
        query.bindValue(0, hR.objectRequest["vote"].toInt() > 0);
        query.bindValue(1, hR.objectRequest["vote"].toInt() < 0);
        query.bindValue(2, hR.objectRequest["thread"].toInt());

        query.exec();

        bool ok = QSqlDatabase::database(conName).commit();

        hR.objectResponce["code"] = ok ? 0 : 1;
        bool tt = true;
        hR.objectResponce["response"] = ThreadInfo::getFullThreadInfo(hR.objectRequest["thread"].toInt(), tt);
        ;

        hR.prepareOutput();
        response.setStatus(200);
        response.out() << hR.output;
        BdWrapper::closeConnection(conName);
    }
};

class ThreadSubscribe : public Wt::WResource {
public:
    virtual ~ThreadSubscribe()
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
        bool test = QSqlDatabase::database(conName).transaction();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("INSERT INTO Subscribers (user, thread_id) VALUES (?, ?);");
        query.bindValue(0, hR.objectRequest["user"].toString());
        query.bindValue(1, hR.objectRequest["thread"].toInt());

        query.exec();

        bool ok = QSqlDatabase::database(conName).commit();

        hR.responseContent["thread"] = hR.objectRequest["thread"];
        hR.responseContent["user"] = hR.objectRequest["user"];

        hR.objectResponce["code"] = ok ? 0 : 1;
        hR.objectResponce["response"] = hR.responseContent;

        hR.prepareOutput();
        response.setStatus(200);
        response.out() << hR.output;
        BdWrapper::closeConnection(conName);
    }
};

class ThreadUnSubscribe : public Wt::WResource {
public:
    virtual ~ThreadUnSubscribe()
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
        bool test = QSqlDatabase::database(conName).transaction();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("DELETE FROM Subscribers WHERE user=? AND thread_id=?;");
        query.bindValue(0, hR.objectRequest["user"].toString());
        query.bindValue(1, hR.objectRequest["thread"].toInt());

        query.exec();

        bool ok = QSqlDatabase::database(conName).commit();

        hR.responseContent["thread"] = hR.objectRequest["thread"];
        hR.responseContent["user"] = hR.objectRequest["user"];

        hR.objectResponce["code"] = ok ? 0 : 1;
        hR.objectResponce["response"] = hR.responseContent;

        hR.prepareOutput();
        response.setStatus(200);
        response.out() << hR.output;
        std::cout << hR.output << " output";
        BdWrapper::closeConnection(conName);
    }
};
class ThreadList : public Wt::WResource {
public:
    virtual ~ThreadList()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestList hR;
        QString userOrForum;
        bool isForum = false;
        userOrForum = userOrForum.fromStdString(request.getParameter("user") ? *request.getParameter("user") : " ");
        if (userOrForum == " ") {
            isForum = true;
            userOrForum = userOrForum.fromStdString(request.getParameter("forum") ? *request.getParameter("forum") : " ");
        }
        QString order;
        order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : " ");
        //0 - magic constant for empty parametr
        QString since_id;
        since_id = userOrForum.fromStdString(request.getParameter("since") ? *request.getParameter("since") : " ");
        QString limit;
        limit = userOrForum.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : " ");

        QString str_since;
        QString str_limit;
        QString str_order;
        QString quote = "\"";

        if (since_id != " ")
            str_since = " AND date >= " + quote + since_id + quote;

        if (limit != " ")
            str_limit = " LIMIT " + limit;
        if (order == "asc")
            str_order = " ORDER BY date asc ";
        else
            str_order = " ORDER BY date desc ";
        QString conName = BdWrapper::getConnection();
        QSqlQuery query(QSqlDatabase::database(conName));
        QString expression;
        if (!isForum) {
            expression = "SELECT * FROM Threads WHERE user=" + quote + userOrForum + quote + str_since + str_order + str_limit + ";";

        } else {
            expression = "SELECT * FROM Threads WHERE forum=" + quote + userOrForum + quote + str_since + str_order + str_limit + ";";
        }
        bool ok = query.exec(expression);
        // std::cout << query.lastQuery().toStdString() << "hh_";

        hR.handleResponse();
        QJsonArray arrayOfThreads;
        bool isThreadExist = true; // заглушка

        if (ok) {
            while (query.next()) {
                int id = query.value(0).toInt();
                QJsonObject jsonObj = ThreadInfo::getFullThreadInfo(id, isThreadExist); // assume this has been populated with Json data
                arrayOfThreads << jsonObj;
            }
        }
        hR.objectResponce["response"] = arrayOfThreads;

        hR.prepareOutput();

        response.setStatus(200);

        response.out() << hR.output;
        BdWrapper::closeConnection(conName);
    }
};

class ThreadListPost : public Wt::WResource {
public:
    virtual ~ThreadListPost()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        HandleRequestList hR;
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
                QString conName = BdWrapper::getConnection();
                QSqlQuery query2(QSqlDatabase::database(conName));
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
                BdWrapper::closeConnection(conName);
            }

            if (order == "desc" || order == " ") {
                //очень грустный момент
                QString conName = BdWrapper::getConnection();
                QSqlQuery query2(QSqlDatabase::database(conName));
                QString expression = "SELECT Count(path) FROM Posts p WHERE p.thread_id=" + quote + thread + quote + str_since + ";";
                query2.exec(expression);
                query2.next();
                int max_value = query2.value(0).toInt();
                QString tmp = QString::number(max_value - limit.toInt() - 1, BASE);

                tmp = tmp + "0" + "0" + "0" + "0" + "0";
                str_sort = " AND (path >" + quote + tmp + quote + ")";
                str_sort += " order by path desc";
                str_limit = " ";
                BdWrapper::closeConnection(conName);
            }
            str_order = " ";
        }
        // str_order = "";
        QString conName = BdWrapper::getConnection();
        QSqlQuery query(QSqlDatabase::database(conName));
        QString expression;
        expression = "SELECT p.date, p.dislikes, p.forum, p.id, p.isApproved, p.isDeleted, p.isEdited, p.isHighlighted, p.isSpam, p.likes, p.message, p.thread_id, p.user, p.parent, p.likes-p.dislikes as points FROM Posts p WHERE p.thread_id=" + quote + thread + quote + str_since + str_sort + str_order + str_limit + ";";

        bool ok = query.exec(expression);
        //std::cout << query.lastQuery().toStdString() << "HEI2";
        hR.handleResponse();
        QJsonArray arrayOfPosts;
        bool isPostExist = true; // заглушка

        if (ok) {
            while (query.next()) {
                int id = query.value(3).toInt();
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
#endif // THREAD_H

