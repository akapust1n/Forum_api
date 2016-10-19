#ifndef FORUMINFO_H
#define FORUMINFO_H
#include "PostInfo.h"
#include "Trash.h"
#include "UserInfo.h"
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
#include <BdWrapper.h>

class ForumInfo {
public:
    static QJsonObject getForumCreateInfo(QString name, bool& isForumExist)
    {
        QString strGoodReply = Source::getForumTemplate();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        //  QJsonObject objectResponce = jsonResponse.object();
        QJsonObject jsonArray = jsonResponse.object();
        QString conName = BdWrapper::getConnection();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("SELECT * FROM Forums WHERE short_name=:name;");
        query.bindValue(":name", name);
        query.exec();
        bool ok = query.next();
        //   QJsonObject jsonArray;
        if (ok) {

            jsonArray["id"] = query.value(0).toInt();
            jsonArray["name"] = query.value(1).toString();
            jsonArray["short_name"] = query.value(2).toString();
            jsonArray["user"] = query.value(3).toString();

            isForumExist = true;

            //
        } else {
            isForumExist = false;
        }
        BdWrapper::closeConnection(conName);

        return jsonArray;
    }
    static QJsonObject getFullForumInfo(QString name, bool& isForumExist)
    {
        QString strGoodReply = Source::getForumTemplate();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        //  QJsonObject objectResponce = jsonResponse.object();
        QJsonObject jsonArray = jsonResponse.object();
        QString conName = BdWrapper::getConnection();
        QSqlQuery query(QSqlDatabase::database(conName));
        query.prepare("SELECT * FROM Forums WHERE short_name=:name;");
        query.bindValue(":name", name);
        query.exec();

        bool ok = query.next();
        //   QJsonObject jsonArray;
        if (ok) {
            jsonArray["id"] = query.value(0).toInt();
            jsonArray["name"] = query.value(1).toString();
            jsonArray["short_name"] = query.value(2).toString();
            jsonArray["user"] = UserInfo::getFullUserInfo(query.value(3).toString(), isForumExist);
            isForumExist = true;
            //
        } else {
            isForumExist = false;
        }
        BdWrapper::closeConnection(conName);

        return jsonArray;
    }
};

#endif // FORUMINFO_H
