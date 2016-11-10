#include "HandleTemplates.h"

void HandleRequestBase::handlePostParams(const Wt::Http::Request &request)
{
    QString params = LineAnalyze::getRequestBody(request);
    QJsonDocument jsonRequest = QJsonDocument::fromJson(params.toUtf8());

    objectRequest = jsonRequest.object();
}

void HandleRequestBase::handleResponse()
{
    QString strGoodReply = Source::getAnswerTemplate();

    QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
    objectResponce = jsonResponse.object();

    responseContent = objectResponce["response"].toObject();
}

void HandleRequestBase::prepareOutput()
{
    QJsonDocument doc(objectResponce);
    //       QJsonObject jsonObj; // assume this has been populated with Json data

    QString strJson(doc.toJson(QJsonDocument::Compact));
    output = strJson.toStdString();

}



void HandleRequestList::handleResponse()
{
    QString strGoodReply = Source::getAnswerTemplateList();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
    objectResponce = jsonResponse.object();
}

void HandleRequestList::prepareOutput()
{
    QJsonDocument doc(objectResponce);
    //       QJsonObject jsonObj; // assume this has been populated with Json data

    QString strJson(doc.toJson(QJsonDocument::Compact));
    output = strJson.toStdString();

}

#include "HandleTemplates.h"
