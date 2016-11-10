#ifndef HANDLETEMPLATES_H
#define HANDLETEMPLATES_H
#include "PostInfo.h"
#include "Trash.h"
#include <QJsonArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValue>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <Source.h>
#include <UserInfo.h>
#include <Wt/Http/Response>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Json/Serializer>
#include <Wt/WResource>
#include <Wt/WServer>
#include <iostream>

class HandleRequestBase {
public:
    QJsonObject objectRequest;
    QJsonObject objectResponce;
    QJsonObject responseContent; //jsonArray
    std::string output;

public:
    void handlePostParams(const Wt::Http::Request& request);
    virtual void handleResponse();
    virtual void prepareOutput();
};
class HandleRequestList : public HandleRequestBase {

public:
    virtual void handleResponse();
    virtual void prepareOutput();
};

#endif // HANDLETEMPLATES_H

