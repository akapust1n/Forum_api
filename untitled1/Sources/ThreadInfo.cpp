#include "ThreadInfo.h"
#include <zdb/zdb.h>

extern ConnectionPool_T pool;

QJsonObject ThreadInfo::getThreadCreateInfo(int id, bool& isThreadExist)
{
    QString strGoodReply = Source::getThreadCreateTemplate();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
    QJsonObject jsonArray = jsonResponse.object();

    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T p = Connection_prepareStatement(con, "SELECT * FROM Threads WHERE id=?;");
    PreparedStatement_setInt(p, 1, id);
    ResultSet_T result;

    TRY
    {
        result = PreparedStatement_executeQuery(p);
        while (ResultSet_next(result)) {

            jsonArray["id"] = ResultSet_getInt(result, 1);
            jsonArray["forum"] = ResultSet_getString(result, 2);
            jsonArray["user"] = ResultSet_getString(result, 3);
            jsonArray["title"] = ResultSet_getString(result, 4);
            jsonArray["slug"] = ResultSet_getString(result, 5);
            jsonArray["message"] = ResultSet_getString(result, 6);

            auto date = ResultSet_getDateTime(result, 7);

            // jsonArray["date"] = query.value(6).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
            jsonArray["date"] = Convertor::getTime(date);

            jsonArray["isClosed"] = ResultSet_getInt(result, 10);
            jsonArray["isDeleted"] = ResultSet_getInt(result, 11);

            isThreadExist = true;
        }
    }
    CATCH(SQLException)
    {
        isThreadExist = false;

        std::cerr << "smth is wrong";
    }
    END_TRY;
    Connection_close(con);

    return jsonArray;
}

QJsonObject ThreadInfo::getFullThreadInfo(int id, bool& isThreadExist)
{
    QString strGoodReply = Source::getFullThreadTemplate();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
    QJsonObject jsonArray = jsonResponse.object();
    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T p = Connection_prepareStatement(con, "SELECT id,forum,user,title,slug,message,date,likes,dislikes,isClosed,isDeleted FROM Threads WHERE id=?");
    PreparedStatement_setInt(p, 1, id);
    ResultSet_T result;

    TRY
    {
        result = PreparedStatement_executeQuery(p);
        while (ResultSet_next(result)) {

            jsonArray["id"] = ResultSet_getInt(result, 1);
            jsonArray["forum"] = ResultSet_getString(result, 2);
            jsonArray["user"] = ResultSet_getString(result, 3);
            jsonArray["title"] = ResultSet_getString(result, 4);
            jsonArray["slug"] = ResultSet_getString(result, 5);
            jsonArray["message"] = ResultSet_getString(result, 6);

            auto date = ResultSet_getDateTime(result, 7);

            // jsonArray["date"] = query.value(6).toDateTime().toString("yyyy-MM-dd hSh:mm:ss");
            jsonArray["date"] = Convertor::getTime(date);

            jsonArray["likes"] = ResultSet_getInt(result, 8);
            jsonArray["dislikes"] = ResultSet_getInt(result, 9);
            jsonArray["isClosed"] = ResultSet_getInt(result, 10);
            ;
            jsonArray["isDeleted"] = ResultSet_getInt(result, 11);
            jsonArray["points"] = ResultSet_getInt(result, 8) - ResultSet_getInt(result, 9);
            jsonArray["posts"] = PostInfo::countPosts(id);
            isThreadExist = true;
        }
    }
    CATCH(SQLException)
    {
        isThreadExist = false;

        std::cerr << "smth is wrong";
    }
    END_TRY;
    Connection_close(con);


    return jsonArray;
}
