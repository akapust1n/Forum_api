#ifndef THREADINFO_H
#define THREADINFO_H
#include "PostInfo.h"
#include "Trash.h"
#include <BdWrapper.h>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariant>
#include <Source.h>
#include <Wt/Http/Response>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Json/Serializer>
#include <Wt/WResource>
#include <Wt/WServer>
#include <iostream>
#include <qvector.h>

class ThreadInfo {
public:
    static QJsonObject getThreadCreateInfo(int id, bool& isThreadExist)
    {
        QString strGoodReply = Source::getThreadCreateTemplate();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        //  QJsonObject hR.objectResponce = jsonResponse.object();
        QJsonObject jsonArray = jsonResponse.object();
        QString conName = BdWrapper::getConnection();
        QSqlQuery query(QSqlDatabase::database(conName));

        query.prepare("SELECT * FROM Threads WHERE id=:id;");
        query.bindValue(":id", id);
        query.exec();

        bool ok = query.next();
        //   QJsonObject jsonArray;
        if (ok) {

            jsonArray["id"] = query.value(0).toInt();
            jsonArray["forum"] = query.value(1).toString();
            jsonArray["user"] = query.value(2).toString();
            jsonArray["title"] = query.value(3).toString();
            jsonArray["slug"] = query.value(4).toString();
            jsonArray["message"] = query.value(5).toString();

            jsonArray["date"] = query.value(6).toDateTime().toString("yyyy-MM-dd hh:mm:ss");

            jsonArray["isClosed"] = query.value(9).toBool();
            jsonArray["isDeleted"] = query.value(10).toBool();

            isThreadExist = true;

            //
        } else {
            isThreadExist = false;
        }
        BdWrapper::closeConnection(conName);

        return jsonArray;
    }
    static QJsonObject getFullThreadInfo(int id, bool& isThreadExist)
    {
        QString strGoodReply = Source::getFullThreadTemplate();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        //  QJsonObject hR.objectResponce = jsonResponse.object();
        QJsonObject jsonArray = jsonResponse.object();
        QString conName = BdWrapper::getConnection();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("SELECT id,forum,user,title,slug,message,date,likes,dislikes,isClosed,isDeleted FROM Threads WHERE id=:id;");
        query.bindValue(":id", id);
        query.exec();

        bool ok = query.next();
        //   QJsonObject jsonArray;
        if (ok) {

            jsonArray["id"] = query.value(0).toInt();
            jsonArray["forum"] = query.value(1).toString();
            jsonArray["user"] = query.value(2).toString();
            jsonArray["title"] = query.value(3).toString();
            jsonArray["slug"] = query.value(4).toString();
            jsonArray["message"] = query.value(5).toString();

            jsonArray["date"] = query.value(6).toDateTime().toString("yyyy-MM-dd hh:mm:ss");

            jsonArray["likes"] = query.value(7).toInt();
            jsonArray["dislikes"] = query.value(8).toInt();
            jsonArray["isClosed"] = query.value(9).toBool();
            jsonArray["isDeleted"] = query.value(10).toBool();
            jsonArray["points"] = query.value(7).toInt() - query.value(8).toInt();
            jsonArray["posts"] = PostInfo::countPosts(id);
            isThreadExist = true;

            //
        } else {
            isThreadExist = false;
        }
        BdWrapper::closeConnection(conName);

        return jsonArray;
    }
    /*
    static QJsonObject getFullThreadInfo(QString forum, bool& isThreadExist)
    {
        QString strGoodReply = Source::getThreadTemplate();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        //  QJsonObject hR.objectResponce = jsonResponse.object();
        QJsonObject jsonArray = jsonResponse.object();
        QSqlQuery query(QSqlDatabase::database(BdWrapper::getConnection()));
        query.prepare("SELECT * FROM Threads WHERE forum=:id;");
        query.bindValue(":id", forum);
        query.exec();
        bool ok = query.next();
        //   QJsonObject jsonArray;
        if (ok) {
            jsonArray["id"] = query.value(0).toInt();
            jsonArray["forum"] = query.value(1).toString();
            jsonArray["user"] = query.value(2).toString();
            jsonArray["title"] = query.value(3).toString();
            jsonArray["slug"] = query.value(4).toString();
            jsonArray["message"] = query.value(5).toString();
            jsonArray["date"] = query.value(6).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
            jsonArray["likes"] = query.value(7).toInt();
            jsonArray["dislikes"] = query.value(8).toInt();
            jsonArray["isClosed"] = query.value(9).toBool();
            jsonArray["isDeleted"] = query.value(10).toBool();

            isThreadExist = true;

            //
        } else {
            isThreadExist = false;
        }

        return jsonArray;
    }*/
};

#endif // THREADINFO_H
