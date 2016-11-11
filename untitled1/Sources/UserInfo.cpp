#include "UserInfo.h"
#include <QtGlobal>
#include <zdb/zdb.h>

extern ConnectionPool_T pool;

auto UserInsideInfo::_getFollowers(QString folowee)
{
    Connection_T con = ConnectionPool_getConnection(pool);
    result.clear(); //плохо с точки зрения памяти и скорости
    PreparedStatement_T p = Connection_prepareStatement(con,
        "SELECT follower FROM Followers WHERE followee=?");
    const std::string _folowee  = folowee.toStdString();
    PreparedStatement_setString(p, 1, _folowee.c_str());
    ResultSet_T _result;
    bool ok = true;

    TRY
    {
        _result = PreparedStatement_executeQuery(p);
        while (ResultSet_next(_result)) {
            result.append(ResultSet_getString(_result, 1));
        }
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "getFollowers error";

    }
    END_TRY;
    Connection_close(con);

    return result;
}

auto UserInsideInfo::_getFollowee(QString follower)
{
    Connection_T con = ConnectionPool_getConnection(pool);
    result.clear(); //плохо с точки зрения памяти и скорости
    PreparedStatement_T p = Connection_prepareStatement(con, "SELECT followee FROM Followers WHERE follower=?;");
    const std::string _follower = follower.toStdString();
    PreparedStatement_setString(p, 1, _follower.c_str());
    ResultSet_T _result;
    bool ok = true;

    TRY
    {
        _result = PreparedStatement_executeQuery(p);
        while (ResultSet_next(_result)) {
            result.append(ResultSet_getString(_result, 1));
        }
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "getFollowee error";
    }
    END_TRY;
    Connection_close(con);

    return result;
}

auto UserInsideInfo::_getSubscriptions(QString user)
{
    Connection_T con = ConnectionPool_getConnection(pool);
    result.clear(); //плохо с точки зрения памяти и скорости
    PreparedStatement_T p = Connection_prepareStatement(con, "SELECT thread_id FROM Subscribers WHERE user=?");
    const std::string _user = user.toStdString();
    PreparedStatement_setString(p,1, _user.c_str());
    ResultSet_T _result;
    bool ok = true;

    TRY
    {
        _result = PreparedStatement_executeQuery(p);
        while (ResultSet_next(_result)) {
            result_int.append(ResultSet_getInt(_result, 1));
        }
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "getSubscriptions error";
    }
    END_TRY;
    Connection_close(con);

    return result_int;
}

QJsonArray UserInsideInfo::getFollowers(QString folowee)
{
    auto temp = _getFollowers(folowee);
    QJsonArray result;
    QString str;
    for (int i = 0; i < temp.size(); i++) {
        str = temp[i];
        result.append(str);
    }
    return result;
}

QJsonArray UserInsideInfo::getFollowee(QString follower)
{
    auto temp = _getFollowee(follower);
    QJsonArray result;
    QString str;
    for (int i = 0; i < temp.size(); i++) {
        str = temp[i];
        result.append(str);
    }
    return result;
}

QJsonArray UserInsideInfo::getSubscriptions(QString follower)
{
    auto temp = _getSubscriptions(follower);
    QJsonArray result;
    int str;
    for (int i = 0; i < temp.size(); i++) {
        str = temp[i];
        result.append(str);
    }
    return result;
}

QJsonObject UserInfo::getUserInfo(QString email, bool& isUserExist)
{

    QString strGoodReply = Source::getUserTemplate();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
    QJsonObject jsonArray = jsonResponse.object();

    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;
    ResultSet_T result;
    PreparedStatement_T p;
    const std::string _email = email.toStdString();
    TRY
    {
        p = Connection_prepareStatement(con, "SELECT * FROM Users WHERE email=?");
        PreparedStatement_setString(p, 1, _email.c_str());
    }
    CATCH(SQLException)
    {
        std::cerr << "prepare error";
    }

    END_TRY;

    TRY
    {
        result = PreparedStatement_executeQuery(p);

        while (ResultSet_next(result)) {
            jsonArray["id"] = ResultSet_getInt(result, 1);
            QString temp = ResultSet_getString(result, 2);
            if (temp.length() == 0)
                jsonArray["username"] = QJsonValue::Null;
            else
                jsonArray["email"] = ResultSet_getString(result, 2);

            temp = ResultSet_getString(result, 3);
            if (temp.length() == 0)
                jsonArray["username"] = QJsonValue::Null;
            else
                jsonArray["username"] = ResultSet_getString(result, 3);

            jsonArray["about"] = ResultSet_getString(result, 4);
            if (jsonArray["about"] == "")
                jsonArray["about"] = QJsonValue::Null;

            jsonArray["name"] = ResultSet_getString(result, 5);
            if (jsonArray["name"] == "")
                jsonArray["name"] = QJsonValue::Null;

            jsonArray["isAnonymous"] = ResultSet_getInt(result, 6);
            isUserExist = true;
        }
    }
    CATCH(SQLException)
    {
        ok = false;
        isUserExist = false;
        std::cerr << "GETUSER error";
    }
    END_TRY;
    Connection_close(con);
    return jsonArray;
}

QJsonObject UserInfo::getFullUserInfo(QString email, bool& isUserExist)
{
    QJsonObject jsonArray = getUserInfo(email, isUserExist);
    UserInsideInfo userInsideInfo;
    if (isUserExist) {
        QJsonArray followers = userInsideInfo.getFollowers(email);
        QJsonArray followee = userInsideInfo.getFollowee(email);

        jsonArray["following"] = followee;
        jsonArray["followers"] = followers;

        QJsonArray subscriptions = userInsideInfo.getSubscriptions(email);
        jsonArray["subscriptions"] = subscriptions;
    }
    return jsonArray;
}
