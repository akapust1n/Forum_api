#ifndef DB_H
#define DB_H
#include <QString>
#include <Wt/Http/Response>
#include <Wt/WResource>
#include <Wt/WServer>
#include <QSqlQuery>
#include "BdWrapper.h"
#include  <Wt/Json/Object>
#include <Wt/Json/Serializer>

class DbClear : public Wt::WResource {
public:

    virtual ~DbClear()
    {
        beingDeleted();
    }


protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        QString turn_off = "SET foreign_key_checks = 0;";
        QString qStr ="SHOW TABLES;";
        QString qStr1 = "TRUNCATE TABLE Users;";
        QString qStr2 = "TRUNCATE TABLE Forums;";
        QString qStr3 = "TRUNCATE TABLE Threads;";
        QString qStr4 = "TRUNCATE TABLE Posts;";
        QString qStr5 = "TRUNCATE TABLE Followers;";
        QString qStr6 = "TRUNCATE TABLE Subscribers;";
        QString turn_on = "SET foreign_key_checks = 1;";
        QSqlQuery query;
        BdWrapper::execute_sql("SHOW TABLES;");
        if( BdWrapper::execute_sql(qStr)){
               Wt::Json::Object data;

               data["code"] = 0;
               data["response"] = "OK";
               response.setStatus(200);
               response.setMimeType("application/json");
              // response.addHeader("req", serialize(data));


              response.out()<< serialize(data);
        }
       //response.out() << "Db clear!\n";
    }

};
#endif // DB_H
