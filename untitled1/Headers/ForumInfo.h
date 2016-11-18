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

class ForumInfo {
public:
    static QJsonObject getForumCreateInfo(QString name, bool& isForumExist);
    static QJsonObject getFullForumInfo(QString name, bool& isForumExist);
    static int getForumID(const std::string forum);
};

#endif // FORUMINFO_H

