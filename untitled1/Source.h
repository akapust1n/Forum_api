#ifndef SOURCE_H
#define SOURCE_H
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
    static QString getAnswerTemplateList()
    {
        QString answer = "{"
                         "\"code\": 0,"
                         "\"response\":["
                         "]"
                         " }";
        return answer;
    }
    static QString getUserTemplate()
    {
        QString answer = "{"
                         "\"about\": \"doNOT_SHOW\","
                         "\"email\": \"example@mail.ru\","
                         "\"followers\": [],"
                         "\"following\": [],"
                         "\"id\": -1,"
                         "\"isAnonymous\": false,"
                         "\"name\": \"John\","
                         "\"subscriptions\": [],"
                         "\"username\": \"user1\""
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
    static QString getThreadTemplate()
    {
        QString answer = "{"
                         "\"date\": \"2014-01-01 00:00:01\","
                         "\"forum\": \"forumTEMPLATE\","
                         "\"id\": 1,"
                         "\"isClosed\": true,"
                         "\"isDeleted\": false,"
                         "\"message\": \"hey hey hey hey!\","
                         "\"slug\": \"forumTEMPLATE\","
                         "\"title\": \"TforumTEMPLATE\","
                         "\"user\": \"forumTEMPLATE\""
                         "}";
        return answer;
    }
    static QString getPostTemplate()
    {
        QString answer =  "{"
            "\"date\": \"2014-01-02 00:02:01\","
            "\"dislikes\": 0,"
            "\"forum\": \"forum2\","
            "\"id\": 3,"
            "\"isApproved\": false,"
            "\"isDeleted\": true,"
            "\"isEdited\": true,"
            "\"isHighlighted\": false,"
            "\"isSpam\": true,"
            "\"likes\": 0,"
            "\"message\": \"my message 1\","
            "\"parent\": \"\""
            "\"points\": 0,"
            "\"thread\": \"\","
            "\"user\": \"example@mail.ru\""
        "}";
        return answer;
    }
};

#endif // SOURCE_H
