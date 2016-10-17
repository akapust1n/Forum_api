#ifndef HANDLETEMPLATES_H
#define HANDLETEMPLATES_H
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
#include "PostInfo.h"

class HandleRequestBase {
protected:
    QJsonObject objectRequest;
    QJsonObject objectResponce;
    QJsonObject responseContent; //jsonArray
    std::string output;

protected:
    // virtual bool doSqlRequest();
    void handlePostParams(const Wt::Http::Request& request)
    {
        QString params = LineAnalyze::getRequestBody(request);
        QJsonDocument jsonRequest = QJsonDocument::fromJson(params.toUtf8());

        objectRequest = jsonRequest.object();
    }
    virtual void handleResponse()
    {
        QString strGoodReply = Source::getAnswerTemplate();

        QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
        objectResponce = jsonResponse.object();

        responseContent = objectResponce["response"].toObject();
    }
    virtual void prepareOutput()
    {
        QJsonDocument doc(objectResponce);
        QByteArray data = doc.toJson();
        output = data.toStdString();
    }
};
class HandleRequestList : public HandleRequestBase {

protected:
    virtual void handleResponse(){
    QString strGoodReply = Source::getAnswerTemplateList();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
     objectResponce = jsonResponse.object();

    }
    virtual void prepareOutput()
    {
        QJsonDocument doc(objectResponce);
        QByteArray data = doc.toJson();
        output = data.toStdString();
    }
};

#endif // HANDLETEMPLATES_H
