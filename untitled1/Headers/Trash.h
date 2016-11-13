#ifndef ENCODING_H
#define ENCODING_H
#include <QJsonValue>
#include <QString>
#include <QVariant>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <boost/any.hpp>
#include <string>
#include <time.h>
#include <iostream>

class Convertor {
public:
    //translate tm to yyyy-MM-dd hh:mm:ss");

    static QString getTime(tm time)
    {
        QString result;
        result.append(QString::number(time.tm_year));
        result.append("-");
        result.append(doubleNumber(time.tm_mon+1)); //почему +1? Потому что месяца тут 0-11, а надо 1-12
        result.append("-");
        result.append(doubleNumber(time.tm_mday));
        result.append(" ");
        result.append(doubleNumber(time.tm_hour));
        result.append(":");
        result.append(doubleNumber(time.tm_min));
        result.append(":");
        result.append(doubleNumber(time.tm_sec));
        return result;
    }

private:
    static QString doubleNumber(int num)
    {
        QString result;
        if (num < 10)
            result.append("0");
        result.append(QString::number(num));
        return result;
    }
};

class LineAnalyze {
public:
    //TODO
    static boost::any checkCorrectString(QString line)
    {
        if (line == "")
            return QJsonValue::Null;
        else
            return line;
    }
    static QString getRequestBody(const Wt::Http::Request& request)
    {
        QString params;
        std::string temp;
        while (std::getline(request.in(), temp))
            ;
        params.fromStdString(temp);
        try {
            params = LineAnalyze::fixUnicode(temp);
        } catch (...) { //тут должен быть выброс ошибки :)
        }
        return params;
    }

private:
    static QString fixUnicode(std::string line)
    {
        QString temp;

        //auto temp2 = request.getParameter("first");
        temp = temp.fromStdString(line);

        temp.replace(" u\'", "\'"); //ну это костыль
        temp.replace("\'", "\"");
        temp.replace(":True", ":true");
        temp.replace(": True", ": true");
        temp.replace(": False", ": false");
        temp.replace(":False", ":false");

        // temp.replace("\"\\", "\'\\");
        // temp.replace("u\'","\"u\'");
        return temp;
    }
};

#endif // ENCODING_H
