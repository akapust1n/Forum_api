#ifndef THREADINFO_H
#define THREADINFO_H
#include "Trash.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
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
    static QJsonObject getFullThreadInfo(int id, bool& isThreadExist)
    {
        QString strGoodReply = Source::getThreadTemplate();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        //  QJsonObject objectResponce = jsonResponse.object();
        QJsonObject jsonArray = jsonResponse.object();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
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
            jsonArray["date"] = query.value(6).toString();
            jsonArray["likes"] = query.value(7).toInt();
            jsonArray["dislikes"] = query.value(5).toInt();
            jsonArray["isClosed"] = query.value(6).toBool();
            jsonArray["isDeleted"] = query.value(7).toBool();

            isThreadExist = true;

            //
        } else {
            isThreadExist = false;
        }

        return jsonArray;
    }
};

#endif // THREADINFO_H
