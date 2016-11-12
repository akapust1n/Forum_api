#include "PostInfo.h"
#include <zdb/zdb.h>

extern ConnectionPool_T pool;

int PostInfo::countPosts(int thread_id)
{
    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T p = Connection_prepareStatement(con, "SELECT COUNT(*) FROM Posts WHERE thread_id=? AND isDeleted=false");
    PreparedStatement_setInt(p, 1, thread_id);
    ResultSet_T _result;
    int result;
    TRY
    {
        _result = PreparedStatement_executeQuery(p);
        while (ResultSet_next(_result)) {
            result = ResultSet_getInt(_result, 1);
        }
    }
    CATCH(SQLException)
    {
        std::cerr << "smth is wrong";
    }

    END_TRY;
    Connection_close(con);

    return result;
}
QJsonObject PostInfo::getFullPostInfo(int id, bool& isPostExist)
{
    QString strGoodReply = Source::getPostTemplate();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
    QJsonObject jsonArray = jsonResponse.object();
    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T p = Connection_prepareStatement(con,
        "SELECT id, user, message,forum,thread_id, parent, date,likes, dislikes,isApproved,isHighlighted,isEdited,isSpam,isDeleted FROM Posts WHERE id=?");
    PreparedStatement_setInt(p, 1, id);
    ResultSet_T result;
    TRY
    {
        result = PreparedStatement_executeQuery(p);
        isPostExist = false;
        while (ResultSet_next(result)){
        jsonArray["id"] = ResultSet_getInt(result, 1);
        jsonArray["user"] = ResultSet_getString(result, 2);
        jsonArray["message"] = ResultSet_getString(result, 3);
        jsonArray["forum"] = ResultSet_getString(result, 4);
        jsonArray["thread"] = ResultSet_getInt(result, 5);

        if (!ResultSet_isnull(result, 6))
            jsonArray["parent"] = ResultSet_getInt(result, 6);
        else
            jsonArray["parent"] = QJsonValue::Null;

        auto date = ResultSet_getDateTime(result, 7);

        // jsonArray["date"] = query.value(6).toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        jsonArray["date"] = Convertor::getTime(date);

        jsonArray["likes"] = ResultSet_getInt(result, 8);
        jsonArray["dislikes"] = ResultSet_getInt(result, 9);
        jsonArray["isApproved"] = ResultSet_getInt(result, 10);
        jsonArray["isHighlighted"] = ResultSet_getInt(result, 11);
        jsonArray["isEdited"] = ResultSet_getInt(result, 12);
        jsonArray["isSpam"] = ResultSet_getInt(result, 13);
        jsonArray["isDeleted"] = ResultSet_getInt(result, 14);
        jsonArray["points"] = ResultSet_getInt(result, 8) - ResultSet_getInt(result, 9);
        isPostExist = true;

}
    }
    CATCH(SQLException)
    {
        isPostExist = false;
        std::cerr << "getFullPostInfo_error";
    }
    END_TRY;
    auto tt = Connection_getLastError(con);
    Connection_close(con);

    return jsonArray;
}

QString PostInfo::getPath(int parent_id)
{
    QString result;
    QString path;
    if (parent_id != -1) {
        Connection_T con = ConnectionPool_getConnection(pool);
        PreparedStatement_T p = Connection_prepareStatement(con,
            "SELECT path,parent FROM Posts WHERE id = ? order by path;");
        PreparedStatement_setInt(p, 1, parent_id);
        ResultSet_T _result;

        TRY
        {
            _result = PreparedStatement_executeQuery(p);
            while (ResultSet_next(_result)) {
                path = ResultSet_getString(_result, 1);
            }
        }
        CATCH(SQLException)
        {
            std::cerr << "smth is wrong";
        }
        END_TRY;
    Connection_close(con);

    } else
        path = " ";

    if (path != " ") {
        Connection_T con = ConnectionPool_getConnection(pool);
        PreparedStatement_T p = Connection_prepareStatement(con,
            "SELECT Count(path) FROM Posts WHERE (path LIKE ?) order by path ;");
        std::string st = path.toStdString() + "_";
        PreparedStatement_setString(p, 1, st.c_str());
        ResultSet_T _result;

        TRY
        {
            _result = PreparedStatement_executeQuery(p);
            while (ResultSet_next(_result)) {
                QString temp = ResultSet_getString(_result, 1);
                int value = temp.toInt(0, BASE) + 1;
                result = path + QString::number(value, BASE);
            }
        }
        CATCH(SQLException)
        {
            std::cerr << "smth is wrong";
        }
        END_TRY;
    Connection_close(con);
    }

    else {
        Connection_T con = ConnectionPool_getConnection(pool);
        PreparedStatement_T p = Connection_prepareStatement(con,
            "SELECT Count(path) FROM Posts WHERE (path REGEXP \"^.$\") order by path ;");
        ResultSet_T _result;

        TRY
        {
            _result = PreparedStatement_executeQuery(p);
            while (ResultSet_next(_result)) {
                QString temp = QString::number(ResultSet_getInt(_result,1), BASE);
                int value = temp.toInt(0, BASE) + 1;
                result = QString::number(value, BASE);
            }
        }
        CATCH(SQLException)
        {
            std::cerr << "smth is wrong";
        }
        END_TRY;
    Connection_close(con);
    }

    return result;
}
