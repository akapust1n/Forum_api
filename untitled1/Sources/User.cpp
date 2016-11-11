#include "User.h"
#include <zdb/zdb.h>

extern ConnectionPool_T pool;

UserCreate::~UserCreate()
{
    beingDeleted();
}

void UserCreate::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;
    PreparedStatement_T p = Connection_prepareStatement(con, "INSERT INTO Users (username, about, name, email, isAnonymous) VALUES (?, ?, ?, ?, ?)");
    const std::string username = hR.objectRequest["username"].toString().toStdString();
    PreparedStatement_setString(p, 1, username.c_str());
    const std::string about = hR.objectRequest["about"].toString().toStdString();
    PreparedStatement_setString(p, 2, about.c_str());
    const std::string name = hR.objectRequest["name"].toString().toStdString();
    PreparedStatement_setString(p, 3, name.c_str());
    const std::string email= hR.objectRequest["email"].toString().toStdString();
    PreparedStatement_setString(p, 4, email.c_str());
    PreparedStatement_setInt(p, 5, hR.objectRequest["isAnonymous"].toBool());

    TRY
    {
        PreparedStatement_execute(p);
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "CANT CREATE USER"<<std::endl;
    }
    END_TRY;
    Connection_close(con);

    hR.handleResponse();
    hR.objectResponce["code"] = QJsonValue(ok ? 0 : 5);

    hR.responseContent["username"] = hR.objectRequest["username"];
    hR.responseContent["about"] = hR.objectRequest["about"];
    hR.responseContent["name"] = hR.objectRequest["name"].toString();
    hR.responseContent["email"] = hR.objectRequest["email"];
    hR.responseContent["isAnonymous"] = hR.objectRequest["isAnonymous"];
    bool tt = true;
    hR.objectResponce["response"] = UserInfo::getFullUserInfo(hR.objectRequest["email"].toString(), tt);

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
}

UserDetails::~UserDetails()
{
    beingDeleted();
}

void UserDetails::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    //ПЕРЕПИСАТЬ В USERINFO ВСЕ ЗАПРОСЫ ПРО ЮЗЕРА ((
    //на данный момент это просто захардкожено для прикола
    //Возможно, уже переписал
    QString user;
    user = user.fromStdString(request.getParameter("user") ? *request.getParameter("user") : "");

    QString strGoodReply = Source::getAnswerTemplate();
    bool isUserExist = false;

    hR.handleResponse();
    hR.responseContent = UserInfo::getFullUserInfo(user, isUserExist);
    hR.objectResponce["response"] = hR.responseContent;

    hR.objectResponce["code"] = isUserExist ? 0 : 1;

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
}

UserFollow::~UserFollow()
{
    beingDeleted();
}

void UserFollow::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);

    hR.handleResponse();
    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;
    PreparedStatement_T p = Connection_prepareStatement(con, "INSERT INTO Followers (follower, followee) VALUES (?, ?);");
    PreparedStatement_setString(p, 1, hR.objectRequest["follower"].toString().toStdString().c_str());
    PreparedStatement_setString(p, 2, hR.objectRequest["followee"].toString().toStdString().c_str());
    TRY
    {
        PreparedStatement_execute(p);
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "smth is wrong";
    }
    END_TRY;
    Connection_close(con);


    bool isUserExist = false;
    QJsonObject responseContent = UserInfo::getFullUserInfo(hR.objectRequest["follower"].toString(), isUserExist);
    if (isUserExist) {
        hR.objectResponce["code"] = 0;
        hR.objectResponce["response"] = responseContent;//hr.responseContenc
    } else {
        hR.objectResponce["code"] = 1;
        hR.objectResponce["response"] = "error message";
    }

    hR.prepareOutput();
    response.setStatus(200);

    response.out() << hR.output;
}

UserUnFollow::~UserUnFollow()
{
    beingDeleted();
}

void UserUnFollow::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);

    hR.handleResponse();
    //QString conName = BdWrapper::getConnection();

    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;
    PreparedStatement_T p = Connection_prepareStatement(con, "DELETE FROM Followers WHERE follower=? AND followee=?");
    PreparedStatement_setString(p, 1, hR.objectRequest["follower"].toString().toStdString().c_str());
    PreparedStatement_setString(p, 2, hR.objectRequest["followee"].toString().toStdString().c_str());
    TRY
    {
        PreparedStatement_execute(p);
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "smth is wrong";
    }
    END_TRY;
    Connection_close(con);


    bool isUserExist = false;
    QJsonObject responseContent = UserInfo::getFullUserInfo(hR.objectRequest["follower"].toString(), isUserExist);
    if (isUserExist) {
        hR.objectResponce["code"] = 0;
        hR.objectResponce["response"] = hR.responseContent;
    } else {
        hR.objectResponce["code"] = 1;
        hR.objectResponce["response"] = "error message";
    }

    hR.prepareOutput();
    response.setStatus(200);

    response.out() << hR.output;
}

UserUpdateProfile::~UserUpdateProfile()
{
    beingDeleted();
}

void UserUpdateProfile::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    hR.handleResponse();

    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;
    PreparedStatement_T p = Connection_prepareStatement(con, "UPDATE Users SET about=?, name=? WHERE email=?;");
    PreparedStatement_setString(p, 1, hR.objectRequest["user"].toString().toStdString().c_str());
    PreparedStatement_setString(p, 2, hR.objectRequest["about"].toString().toStdString().c_str());
    PreparedStatement_setString(p, 3, hR.objectRequest["name"].toString().toStdString().c_str());

    TRY
    {
        PreparedStatement_execute(p);
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "smth is wrong";
    }
    END_TRY;
    Connection_close(con);


    bool isUserExist = true;
    QJsonObject responseContent = UserInfo::getFullUserInfo(hR.objectRequest["user"].toString(), isUserExist);
    if (isUserExist) {
        hR.objectResponce["code"] = 0;
        hR.objectResponce["response"] = hR.responseContent;
    } else {
        hR.objectResponce["code"] = 1;
        hR.objectResponce["response"] = "error message";
    }

    hR.prepareOutput();

    response.out() << hR.output;

}

UserListFollowers::~UserListFollowers()
{
    beingDeleted();
}

void UserListFollowers::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestList hR;
    QString user;
    user = user.fromStdString(request.getParameter("user") ? *request.getParameter("user") : " ");

    QString order;
    order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : " ");
    //0 - magic constant for empty parametr
    QString since_id;
    since_id = user.fromStdString(request.getParameter("since_id") ? *request.getParameter("since_id") : " ");
    QString limit;
    limit = user.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : " ");

    QString str_since;
    QString str_limit;
    QString str_order;
    QString quote = "\"";

    if (since_id != " ")
        str_since = " AND id >= " + quote + since_id + quote;

    if (limit != " ")
        str_limit = " LIMIT " + limit;
    if (order == "asc")
        str_order = " ORDER BY name asc ";
    else
        str_order = " ORDER BY name desc";

    hR.handleResponse();
    QJsonArray arrayOfFollowers;
    QString expression;
    expression = "SELECT email FROM Users u JOIN Followers f ON  u.email = f.follower WHERE f.followee=" + quote + user + quote + str_since + str_order + str_limit + ";";
    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;
    ResultSet_T result;
    TRY
    {
        result = Connection_executeQuery(con, expression.toStdString().c_str());
        bool isUserExist = true; // заглушка

        while (ResultSet_next(result)) {
            QString email = ResultSet_getString(result, 1);
            QJsonObject jsonObj = UserInfo::getFullUserInfo(email, isUserExist); // assume this has been populated with Json data
            arrayOfFollowers << jsonObj;
        }
    }

    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "smth is wrong";
    }
    END_TRY;
    Connection_close(con);

    hR.objectResponce["response"] = arrayOfFollowers;

    hR.prepareOutput();

    response.setStatus(200);

    response.out() << hR.output;
}

UserListFollowing::~UserListFollowing()
{
    beingDeleted();
}

void UserListFollowing::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestList hR;
    QString user;
    user = user.fromStdString(request.getParameter("user") ? *request.getParameter("user") : " ");

    QString order;
    order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : " ");
    //0 - magic constant for empty parametr
    QString since_id;
    since_id = user.fromStdString(request.getParameter("since_id") ? *request.getParameter("since_id") : " ");
    QString limit;
    limit = user.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : " ");

    QString str_since;
    QString str_limit;
    QString str_order;
    QString quote = "\"";

    if (since_id != " ")
        str_since = " AND id >= " + quote + since_id + quote;

    if (limit != " ")
        str_limit = " LIMIT " + limit;
    if (order == "asc")
        str_order = " ORDER BY name asc";
    else
        str_order = " ORDER BY name desc";

    hR.handleResponse();
    QJsonArray arrayOfFollowers;
    QString expression;
    expression = "SELECT email FROM Users u JOIN Followers f ON  u.email = f.followee WHERE f.follower=" + quote + user + quote + str_since + str_order + str_limit + ";";
    Connection_T con = ConnectionPool_getConnection(pool);
    ResultSet_T result;
    bool ok = true;
    TRY
    {
        result = Connection_executeQuery(con, expression.toStdString().c_str());

        bool isUserExist = true; // заглушка

        while (ResultSet_next(result)) {
            QString email = ResultSet_getString(result,1);
            QJsonObject jsonObj = UserInfo::getFullUserInfo(email, isUserExist); // assume this has been populated with Json data
            arrayOfFollowers << jsonObj;
        }
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "smth is wrong";
    }
    END_TRY;
    Connection_close(con);

    hR.objectResponce["response"] = arrayOfFollowers;

    hR.prepareOutput();

    response.setStatus(200);

    response.out() << hR.output;
}

UserListPosts::~UserListPosts()
{
    beingDeleted();
}

void UserListPosts::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
            HandleRequestList  hR;
            QString user;
            user = user.fromStdString(request.getParameter("user") ? *request.getParameter("user") : " ");

            QString order;
            order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : " ");
            //0 - magic constant for empty parametr
            QString since_id;
            since_id = user.fromStdString(request.getParameter("since") ? *request.getParameter("since") : " ");
            QString limit;
            limit = user.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : " ");

            QString str_since = " ";
            QString str_limit = " ";
            QString str_order = " ";
            QString quote = "\"";

            if (since_id != " ")
                str_since = " AND date >= " + quote + since_id + quote;
            if (limit != " ")
                str_limit = " LIMIT " + limit;
            if (order == "asc")
                str_order = " ORDER by date asc ";
            else
                str_order = " ORDER by date desc ";

            hR.handleResponse();
            QJsonArray arrayOfPosts;
            QString expression;
            expression = "SELECT p.id FROM Posts p WHERE p.user=" + quote + user + quote + str_since  + str_order + str_limit + ";";
            Connection_T con = ConnectionPool_getConnection(pool);
            bool ok = true;
            ResultSet_T result;

            bool isPostExist = true; // заглушка

            TRY
            {
                result = Connection_executeQuery(con, expression.toStdString().c_str());
                while (ResultSet_next(result)) {
                    int id = ResultSet_getInt(result,1);
                    QJsonObject jsonObj = PostInfo::getFullPostInfo(id, isPostExist); // assume this has been populated with Json data
                    arrayOfPosts << jsonObj;
                }
            }
            CATCH(SQLException)
            {
                ok = false;
                std::cerr << "smth is wrong";
            }
            END_TRY;
            Connection_close(con);

            hR.objectResponce["code"] = ok ? 0 : 1;
            hR.objectResponce["response"] = arrayOfPosts;

            hR.prepareOutput();

            response.setStatus(200);

            response.out() << hR.output;
}
