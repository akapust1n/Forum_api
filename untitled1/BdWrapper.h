#ifndef CONNECTION_H
#define CONNECTION_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <iostream>

class BdWrapper {
public:
    bool createConnection()
    {
        db = QSqlDatabase::addDatabase("QMYSQL", "apidb1");
        db.setHostName("localhost");
        db.setDatabaseName("forum");
        db.setUserName("root");
        db.setPassword("1111");
        bool ok = db.open();

        std::cout << ok << std::endl;
        return ok;
    }
    static bool execute_sql(QString arg)
    {
        QSqlQuery query(QSqlDatabase::database("apidb1"));
        bool result = query.exec(arg);
        std::cout << "execution\n";
        return result;
    }

private:
    QSqlDatabase db;
};

#endif // CONNECTION_H
