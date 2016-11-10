#ifndef DB_H
#define DB_H
#include <QSqlQuery>
#include <QString>
#include <Wt/Http/Response>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Json/Serializer>
#include <Wt/WResource>
#include <Wt/WServer>
#include <zdb/zdb.h>


class DbClear : public Wt::WResource {
public:
    virtual ~DbClear();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

class DbStatus : public Wt::WResource {

public:
    virtual ~DbStatus();

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
};

#endif // DB_H
