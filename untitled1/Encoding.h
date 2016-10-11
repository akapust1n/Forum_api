#ifndef ENCODING_H
#define ENCODING_H
#include <string>
#include <QString>

class Encoding{
public:
    static QString fixUnicode(std::string line){
        QString temp;

        //auto temp2 = request.getParameter("first");
        temp = temp.fromStdString(line);

        temp.replace("\'","\"");
        temp.replace("u\"","u\'");
       // temp.replace("\"\\", "\'\\");
        temp.replace("u\'","\"u\'");
        return temp;
    }
};

#endif // ENCODING_H
