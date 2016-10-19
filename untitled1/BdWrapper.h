#ifndef CONNECTION_H
#define CONNECTION_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <iostream>
#define NUM_CON 10

class BdWrapper {
public:
    static bool createConnection()
    {

            QSqlDatabase  db = QSqlDatabase::addDatabase("QMYSQL", "apidb1");
        db.setHostName("localhost");
        db.setDatabaseName("forum");
        db.setUserName("root");
        db.setPassword("1111");
        bool ok = db.open();


        return ok;
    }
    static bool execute_sql(QString arg)
    {
        QSqlQuery query(QSqlDatabase::database("apidb8"));
        bool result = query.exec(arg);
        std::cout << "execution\n";
        return result;
    }

    static QString getConnection(bool info = 0)
    {
        static int con = 1;
        if (!info) {
            con = con % NUM_CON;
            con += 1;
        }
        QString result = "apidb";
        result += QString::number(con);

        return result;
    }

    //bool create


};

#endif // CONNECTION_H
