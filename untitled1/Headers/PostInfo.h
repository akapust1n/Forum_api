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
    static int countPosts(int thread_id);
    static QJsonObject getFullPostInfo(int id, bool& isPostExist);
    //джва уровня вложенности
    //эта функция в целом ужасна по кода, ПЕРЕПИСАТЬ
    //оно работает
    static QString getPath(int parent_id);
};

#endif // POSTINFO_H

