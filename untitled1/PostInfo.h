#ifndef POSTINFO_H
#define POSTINFO_H
#include "Trash.h"
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

class PostInfo {
public:
    static int countPosts(int thread_id)
    {
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("SELECT COUNT(*) FROM Posts WHERE thread_id=:id AND isDeleted=false;");
        query.bindValue(":id", thread_id);
        bool ok = query.exec();

        int result = 0;
        if ( query.next())
            result = query.value(0).toInt();
        std::cout<<"RESULT_____"<<result<<std::endl;
        return result;
    }
    static QJsonObject getFullPostInfo(int id, bool& isPostExist)
    {
        QString strGoodReply = Source::getPostTemplate();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        //  QJsonObject objectResponce = jsonResponse.object();
        QJsonObject jsonArray = jsonResponse.object();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("SELECT id, user, message,forum,thread_id, parent, date,likes, dislikes,isApproved,isHighlighted,isEdited,isSpam,isDeleted FROM Posts WHERE id=:id;");
        query.bindValue(":id", id);
        query.exec();
        bool ok = query.next();
        //   QJsonObject jsonArray;
        if (ok) {
            // user VARCHAR(50) NOT NULL,
            //            message TEXT NOT NULL,
            //            forum VARCHAR(40),
            //            thread_id INT UNSIGNED NOT NULL,
            //            parent INT NULL DEFAULT NULL,
            //            date TIMESTAMP NOT NULL,
            //            likes INT NOT NULL DEFAULT 0,
            //            dislikes INT NOT NULL DEFAULT 0,
            //            isApproved BOOLEAN NOT NULL DEFAULT 0,
            //            isHighlighted BOOLEAN NOT NULL DEFAULT 0,
            //            isEdited BOOLEAN NOT NULL DEFAULT 0,
            //            isSpam BOOLEAN NOT NULL DEFAULT 0,
            //            isDeleted BOOLEAN NOT NULL DEFAULT 0,
            jsonArray["id"] = query.value(0).toInt();
            jsonArray["user"] = query.value(1).toString();
            jsonArray["message"] = query.value(2).toString();
            jsonArray["forum"] = query.value(3).toString();
            jsonArray["thread"] = query.value(4).toInt();
            // jsonArray["thread"]
            //    = QJsonValue::N;
           if(!query.value(5).isNull())
                jsonArray["parent"] = query.value(5).toInt();
           else
               jsonArray["parent"] = QJsonValue::Null;

            // jsonArray["parent"] = QJsonValue::Null;

            jsonArray["date"] = query.value(6).toDateTime().toString("yyyy-MM-dd hh:mm:ss");

            jsonArray["likes"] = query.value(7).toInt();
            jsonArray["dislikes"] = query.value(8).toInt();
            jsonArray["isApproved"] = query.value(9).toBool();
            jsonArray["isHighlighted"] = query.value(10).toBool();
            jsonArray["isEdited"] = query.value(11).toBool();
            jsonArray["isSpam"] = query.value(12).toBool();
            jsonArray["isDeleted"] = query.value(13).toBool();
            jsonArray["points"] = query.value(7).toInt() - query.value(8).toInt();

            isPostExist = true;

            //
        } else {
            isPostExist = false;
        }

        return jsonArray;
    }
};

#endif // POSTINFO_H
