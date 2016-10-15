#ifndef ENCODING_H
#define ENCODING_H
#include <QJsonValue>
#include <QString>
#include <QVariant>
#include <boost/any.hpp>
#include <string>
#include <Wt/Http/Response>

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
        while (std::getline(request.in(), temp));
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

        temp.replace(" u\'", "\'");   //ну это костыль
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
