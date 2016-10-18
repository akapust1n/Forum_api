#ifndef USERINFO_H
#define USERINFO_H
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

class UserInsideInfo {
private:
    //возвращает список подписчиков
    auto _getFollowers(QString folowee)
    {
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        QVector<QString> result; //плохо с точки зрения памяти и скорости
        query.prepare("SELECT follower FROM Followers WHERE followee=:followee;");
        query.bindValue(":followee", folowee);
        bool ok = query.exec();

        while (query.next()) {
            result.append(query.value(0).toString());
        };

        return result;
    }
    //возвращает список подписок
    auto _getFollowee(QString follower)
    {
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        QVector<QString> result; //плохо с точки зрения памяти и скорости
        query.prepare("SELECT followee FROM Followers WHERE follower=:follower;");
        query.bindValue(":follower", follower);
        query.exec();
        while (query.next()) {
            result.append(query.value(0).toString());
        };
        return result;
    }
    //список подписок на треды
    auto _getSubscriptions(QString user)
    {
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        QVector<int> result; //плохо с точки зрения памяти и скорости
        query.prepare("SELECT thread_id FROM Subscribers WHERE user=:user;");
        query.bindValue(":user", user);
        bool ok = query.exec();
        std::cout<<query.lastQuery().toInt()<<"ddd";

        while (query.next()) {
            result.append(query.value(0).toInt());
        };
        std::cout<<"LEN_"<<result.length();

        return result;
    }

public:
    QJsonArray getFollowers(QString folowee)
    {
        auto temp = _getFollowers(folowee);
        QJsonArray result;
        QString str;
        foreach (str, temp) {
            result.append(str);
        }
        return result;
    }
    QJsonArray getFollowee(QString follower)
    {
        auto temp = _getFollowee(follower);
        QJsonArray result;
        QString str;
        foreach (str, temp) {
            result.append(str);
        }
        return result;
    }

    QJsonArray getSubscriptions(QString follower)
    {
        auto temp = _getSubscriptions(follower);
        QJsonArray result;
        int str;
        foreach (str, temp) {
            result.append(str);
        }
        return result;
    }
};

class UserInfo {
public:
    static QJsonObject getUserInfo(QString email, bool& isUserExist)
    {

        QString strGoodReply = Source::getUserTemplate();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        //  QJsonObject objectResponce = jsonResponse.object();
        QJsonObject jsonArray = jsonResponse.object();
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("SELECT * FROM Users WHERE email=:user;");
        query.bindValue(":user", email);
        query.exec();
        bool ok = query.next();
        //   QJsonObject jsonArray;
        if (ok) {
            // jsonArray["id"] = query.value(0).toInt();
            jsonArray["email"] = query.value(1).toString();
            jsonArray["username"] = query.value(2).toString();
            if (jsonArray["username"] == "")
                jsonArray["username"] = QJsonValue::Null;
            jsonArray["about"] = query.value(3).toString();
            if (jsonArray["about"] == "")
                jsonArray["about"] = QJsonValue::Null;

            jsonArray["name"] = query.value(4).toString();
            if (jsonArray["name"] == "")
                jsonArray["name"] = QJsonValue::Null;

            jsonArray["isAnonymous"] = query.value(5).toBool();
            isUserExist = true;

            //
        } else {
            isUserExist = false;
        }

        return jsonArray;
    }
    static QJsonObject getFullUserInfo(QString email, bool& isUserExist)
    {
        QJsonObject jsonArray = getUserInfo(email, isUserExist);
        UserInsideInfo userInsideInfo;
        if (isUserExist) {

            QJsonArray followers = userInsideInfo.getFollowers(email);
            QJsonArray followee = userInsideInfo.getFollowee(email);
            //   QJsonObject jsonArray;
            jsonArray["following"] = followee;
            jsonArray["followers"] = followers;

            QJsonArray subscriptions = userInsideInfo.getSubscriptions(email);
            jsonArray["subscriptions"] = subscriptions;
            std::cout<<"__"<<jsonArray.length()<<"ARRAY";
        }
        return jsonArray;
    }
    static QJsonObject getFullUserInfoID(int id, bool& isUserExist)
    {
        QJsonObject jsonArray;

        QSqlQuery query(QSqlDatabase::database("apidb1"));
        query.prepare("SELECT email FROM Users WHERE id=:id;");
        query.bindValue(":id", id);
        bool ok = query.exec();
        if (query.next()) {
            QString email = query.value(0).toString();
            jsonArray = getUserInfo(email, isUserExist);
            UserInsideInfo userInsideInfo;
            if (isUserExist) {

                QJsonArray followers = userInsideInfo.getFollowers(email);
                QJsonArray followee = userInsideInfo.getFollowee(email);
                //   QJsonObject jsonArray;
                jsonArray["following"] = followee;
                jsonArray["followers"] = followers;

                QJsonArray subscriptions = userInsideInfo.getSubscriptions(email);
                jsonArray["subscriptions"] = subscriptions;
            }
        }

        return jsonArray;
    }
};

#endif // USERINFO_H
