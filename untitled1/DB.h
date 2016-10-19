#ifndef DB_H
#define DB_H
#include "BdWrapper.h"
#include <QSqlQuery>
#include <QString>
#include <Wt/Http/Response>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Json/Serializer>
#include <Wt/WResource>
#include <Wt/WServer>
#include <BdWrapper.h>

class DbClear : public Wt::WResource {
public:
    virtual ~DbClear()
    {
        beingDeleted();
    };

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        QString turn_off = "SET foreign_key_checks = 0;";

        QString qStr1 = "TRUNCATE TABLE Users;";
        QString qStr2 = "TRUNCATE TABLE Forums;";
        QString qStr3 = "TRUNCATE TABLE Threads;";
        QString qStr4 = "TRUNCATE TABLE Posts;";
        QString qStr5 = "TRUNCATE TABLE Followers;";
        QString qStr6 = "TRUNCATE TABLE Subscribers;";

        QString turn_on = "SET foreign_key_checks = 1;";

        QSqlQuery query(QSqlDatabase::database(BdWrapper::getConnection()));
        bool test = QSqlDatabase::database(BdWrapper::getConnection()).transaction();
        query.exec(turn_off);
        query.exec(qStr1);
        query.exec(qStr2);
        query.exec(qStr3);
        query.exec(qStr4);
        query.exec(qStr5);
        query.exec(qStr6);
        query.exec(turn_on);


        bool ok = QSqlDatabase::database(BdWrapper::getConnection()).commit();

        Wt::Json::Object data;

        data["code"] = Wt::Json::Value(0);
        data["response"] = Wt::Json::Value("OK");
        response.setStatus(200);
        response.setMimeType("application/json");

        response.out() << serialize(data);

        //response.out() << "Db clear!\n";
    }
};

class DbStatus : public Wt::WResource {

public:
    virtual ~DbStatus()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {

        Wt::Json::Object data;

        Wt::Json::parse("{ "
                        "  \"code\": 0, "
                        "  \"response\":{\"user\": 100000, \"thread\": 1000, \"forum\": 100, \"post\": 1000000} "
                        "}",
            data);

        response.setStatus(200);
        response.setMimeType("application/json");
        response.out() << serialize(data);
    }
};

#endif // DB_H
