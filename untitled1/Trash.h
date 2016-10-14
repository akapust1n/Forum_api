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
class Source {
public:
    static QString getAnswerTemplate()
    {
        QString answer = "{"
                         "\"code\": 0,"
                         "\"response\":{"
                         "}"
                         " }";
        return answer;
    }
    static QString getUserTemplate()
    {
        QString answer =  "{"
                          "\"code\": 0,"
                          "\"response\": {"
                          "\"about\": \"doNOT_SHOW\","
                          "\"email\": \"example@mail.ru\","
                          "\"followers\": [],"
                          "\"following\": [],"
                          "\"id\": -1,"
                          "\"isAnonymous\": false,"
                          "\"name\": \"John\","
                          "\"subscriptions\": [],"
                          "\"username\": \"user1\""
                          " }"
                          " }";
       /* QString answer = "{"
           "\"code\": 0,"
            "\"response\": {"
                "\"about\": \"hello im user1\","
                "\"email\": \"example@mail.ru\","
                "\"followers\": ["
                    "\"example3@mail.ru\""
                "],"
                "\"following\": ["
                    "\"example3@mail.ru\":"
                "],"
                "\"id\": 1,"
                "\"isAnonymous\": false,"
                "\"name\": \"John\","
                "\"subscriptions\": ["
                    "4"
                "],"
                "\"username\": \"user1\""
           " }"
        "}";*/
        return answer;
    }
};

#endif // ENCODING_H
