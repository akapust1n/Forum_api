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
    QVector<QString> result;
    QVector<int> result_int;

    //возвращает список подписчиков
    auto _getFollowers(QString folowee);
    //возвращает список подписок
    auto _getFollowee(QString follower);
    //список подписок на треды
    auto _getSubscriptions(QString user);

public:
     ~UserInsideInfo(){
        result.clear();
        result_int.clear();
    }
    QJsonArray getFollowers(QString folowee);
    QJsonArray getFollowee(QString follower);

    QJsonArray getSubscriptions(QString follower);
};

class UserInfo {
public:
    static QJsonObject getUserInfo(QString email, bool& isUserExist);
    static QJsonObject getFullUserInfo(QString email, bool& isUserExist);
};

#endif // USERINFO_H

