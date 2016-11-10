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
    PreparedStatement_setString(p, 1, folowee.toStdString().c_str());
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
    PreparedStatement_setString(p, 1, follower.toStdString().c_str());
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
        std::cerr << "smth is wrong";
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
        std::cerr << "smth is wrong";
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
    std::cout << "TEST1.1" << std::endl;
    PreparedStatement_T p ;
    TRY
    {
        std::cout << "TEST1.1.0." << std::endl;

        p = Connection_prepareStatement(con, "SELECT * FROM Users WHERE email=?;");
        std::cout << "TEST1.1.1." << std::endl;

        PreparedStatement_setString(p, 1, email.toStdString().c_str());
        std::cout << "TEST1.1.2." << std::endl;


    }
    CATCH(SQLException)
    {
        std::cout << "TEST1.2" << std::endl;

        std::cerr << "prepare error";
    }

    END_TRY;

    TRY
    {
        result = PreparedStatement_executeQuery(p);

        while (ResultSet_next(result)) {
            jsonArray["id"] = ResultSet_getInt(result, 1);
            jsonArray["email"] = ResultSet_getString(result, 2);
            if (ResultSet_isnull(result, 3))

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
        std::cerr << "smth is wrong :c";
    }
    FINALLY{
        std::cout<<"finally2"<<std::endl;
    }
    END_TRY;
Connection_close(con);
    return jsonArray;
}

QJsonObject UserInfo::getFullUserInfo(QString email, bool& isUserExist)
{
    QJsonObject jsonArray = getUserInfo(email, isUserExist);
    std::cout << "TEST1.5" << std::endl;
    UserInsideInfo userInsideInfo;
    if (isUserExist) {
        std::cout << "TEST2" << std::endl;
        QJsonArray followers = userInsideInfo.getFollowers(email);
        QJsonArray followee = userInsideInfo.getFollowee(email);
        std::cout << "TEST3" << std::endl;

        jsonArray["following"] = followee;
        jsonArray["followers"] = followers;

        QJsonArray subscriptions = userInsideInfo.getSubscriptions(email);
        jsonArray["subscriptions"] = subscriptions;
    }
    return jsonArray;
}
