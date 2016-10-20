#ifndef CONNECTION_H
#define CONNECTION_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <iostream>
#define NUM_CON 1000000
#define MAX_CONNECTIONS 50
#include <QMutex>
#include <qvector.h>
#include <vector>

std::vector<int> pool(MAX_CONNECTIONS, 0);
QMutex mutex;

class BdWrapper {
public:
    // static QVector<int> vec(5, 42);

    void createConnection()
    {
        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            QString name = QString::number(i);
            QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL", name);
            db.setHostName("localhost");
            db.setDatabaseName("forum");
            db.setUserName("root");
            db.setPassword("1111");
            db.open();
          // std::cout<<"NAME_"<<ok<<name.toStdString();

        }
    }
    static void closeConnection(QString name)
    {
        QMutexLocker locker(&mutex);

        pool[name.toInt()] = 0;
    }

    static QString getConnection()
    {
        QMutexLocker locker(&mutex);
        QString name;
        int freeCon  = 0;

        for(freeCon ; freeCon<pool.size();freeCon++) {
            if (pool[freeCon] == 0){
                //std::cout<<"HERE"<<pool.size();
                name = QString::number(freeCon);
                pool[freeCon] = 1;
                break;

            }
        }
        if(freeCon==MAX_CONNECTIONS) std::cout<<"ALERT";
        return name;
    }

    //bool create
};

#endif // CONNECTION_H
