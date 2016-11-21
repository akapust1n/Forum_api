#include "ForumInfo.h"
#include <zdb/zdb.h>

extern ConnectionPool_T pool;

QJsonObject ForumInfo::getForumCreateInfo(QString name, bool& isForumExist)
{
    QString strGoodReply = Source::getForumTemplate();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
    QJsonObject jsonArray = jsonResponse.object();

    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T p = Connection_prepareStatement(con,
        "SELECT * FROM Forums WHERE short_name=?");
    const std::string _name = name.toStdString();
    PreparedStatement_setString(p, 1, _name.c_str());
    ResultSet_T _result;
    bool ok = true;

    TRY
    {
        _result = PreparedStatement_executeQuery(p);
        while (ResultSet_next(_result)) {

            jsonArray["id"] = ResultSet_getInt(_result, 1);
            jsonArray["name"] = ResultSet_getString(_result, 2);
            jsonArray["short_name"] = ResultSet_getString(_result, 3);
            jsonArray["user"] = ResultSet_getString(_result, 4);
        }
        isForumExist = true;
    }
    CATCH(SQLException)
    {
        isForumExist = false;
        ok = false;
    }
    END_TRY;
    Connection_close(con);
    return jsonArray;
}

QJsonObject ForumInfo::getFullForumInfo(QString name, bool& isForumExist)
{

    QString strGoodReply = Source::getForumTemplate();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
    QJsonObject jsonArray = jsonResponse.object();

    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T p = Connection_prepareStatement(con,
        "SELECT * FROM Forums WHERE short_name=?");
    const std::string _name = name.toStdString();
    PreparedStatement_setString(p, 1, _name.c_str());
    ResultSet_T _result;
    bool ok = true;

    TRY
    {
        _result = PreparedStatement_executeQuery(p);
        while (ResultSet_next(_result)) {

            jsonArray["id"] = ResultSet_getInt(_result, 1);
            jsonArray["name"] = ResultSet_getString(_result, 2);
            jsonArray["short_name"] = ResultSet_getString(_result, 3);
            jsonArray["user"] = UserInfo::getFullUserInfo(ResultSet_getString(_result, 4), isForumExist);
        }
        isForumExist = true;
    }
    CATCH(SQLException)
    {
        isForumExist = false;
        ok = false;
    }
    END_TRY;
    Connection_close(con);
    return jsonArray;
}

int ForumInfo::getForumID(const std::string forum)
{
    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T p = Connection_prepareStatement(con,
        "SELECT id from Forums WHERE short_name=?");
    const std::string _forum = forum;
    int id = 0;
    PreparedStatement_setString(p, 1, _forum.c_str());
    TRY
    {
        ResultSet_T result;
         result = PreparedStatement_executeQuery(p);

        while (ResultSet_next(result))
            id = ResultSet_getInt(result, 1);
    }
    CATCH(SQLException)
    {
        std::cerr << "getForumID ERROR";
    }
    END_TRY;
    Connection_close(con);
    return id;
}
