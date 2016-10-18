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

#define BASE 36

class PostInfo {
public:
    static int countPosts(int thread_id)
    {
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("SELECT COUNT(*) FROM Posts WHERE thread_id=:id AND isDeleted=false;");
        query.bindValue(":id", thread_id);
        bool ok = query.exec();

        int result = 0;
        if (query.next())
            result = query.value(0).toInt();
        std::cout << "RESULT_____" << result << std::endl;
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

            jsonArray["id"] = query.value(0).toInt();
            jsonArray["user"] = query.value(1).toString();
            jsonArray["message"] = query.value(2).toString();
            jsonArray["forum"] = query.value(3).toString();
            jsonArray["thread"] = query.value(4).toInt();
            // jsonArray["thread"]
            //    = QJsonValue::N;
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

            isPostExist = true;

            //
        } else {
            isPostExist = false;
        }

        return jsonArray;
    }
    //джва уровня вложенности
    //эта функция в целом ужасна по кода, ПЕРЕПИСАТЬ
    //оно работает
    static QString getPath(int parent_id)
    {
        QString result = "";
        QString path;
        if (parent_id != -1) {
            QSqlQuery query(QSqlDatabase::database("apidb1"));
            query.prepare("SELECT path,parent FROM Posts WHERE id = ? order by path;");
            query.bindValue(0, parent_id);
            // query.bindValue(1,thread_id);
            bool ok = query.exec();
            query.next();
            path = query.value(0).toString();
        } else
            path = " ";
        std::cout << "PARENT_" << parent_id << "_____" << path.toStdString();
        //второй уровень вложенности
        //        if (parentParentId == 0) {
        if (path != " ") {
            QSqlQuery query2(QSqlDatabase::database("apidb1"));
            query2.prepare("SELECT Count(path) FROM Posts WHERE (path LIKE ?) order by path ;");
            query2.bindValue(0, path + "_");

            bool ok2 = query2.exec();
            query2.next();
            QString temp = query2.value(0).toString();
            int value = temp.toInt(0, BASE) + 1;
            result = path + QString::number(value, BASE);
        } else {
            QSqlQuery query2(QSqlDatabase::database("apidb1"));
            query2.prepare("SELECT Count(path) FROM Posts WHERE (path REGEXP \"^.$\") order by path ;");
            // query2.bindValue(0, "_");

            bool ok2 = query2.exec();
            query2.next();
            QString temp = QString::number(query2.value(0).toInt(), BASE);
            int value = temp.toInt(0, BASE) + 1;
            result = QString::number(value, BASE);
        }

        return result;
    }
};

#endif // POSTINFO_H
