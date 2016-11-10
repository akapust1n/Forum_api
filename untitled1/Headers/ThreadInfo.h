#ifndef THREADINFO_H
#define THREADINFO_H
#include "PostInfo.h"
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

class ThreadInfo {
public:
    static QJsonObject getThreadCreateInfo(int id, bool& isThreadExist);
    static QJsonObject getFullThreadInfo(int id, bool& isThreadExist);
};

#endif // THREADINFO_H

