#ifndef CONNECTION_H
#define CONNECTION_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <iostream>
#define NUM_CON 1000000
#include <QMutex>

class BdWrapper {
public:
    static bool createConnection(QString name)
    {

        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", name);
        db.setHostName("localhost");
        db.setDatabaseName("forum");
        db.setUserName("root");
        db.setPassword("1111");
        bool ok = db.open();
        if (db.isOpen())
            std::cout << "OPEN1";
        return ok;
    }
    static void closeConnection(QString name)
    {

        QSqlDatabase::database(name).close();
        QSqlDatabase::removeDatabase(name);
        if (!QSqlDatabase::database(name).isOpen())
            std::cout << "ClOSE";
    }
    static bool execute_sql(QString arg)
    {
        QSqlQuery query(QSqlDatabase::database("apidb8"));
        bool result = query.exec(arg);
        std::cout << "execution\n";
        return result;
    }

    static QString getConnection()
    {
        static QMutex mutex;
        static int con = 1;
        QMutexLocker locker(&mutex);
        if (con > NUM_CON)
            con = 1;
        else
            con++;
        QString name = QString::number(con);
        createConnection(name); //тут можно кинуть экзепшн

        return name;
    }

    //bool create
};

#endif // CONNECTION_H
