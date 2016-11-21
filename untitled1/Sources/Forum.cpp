#include "Forum.h"

extern ConnectionPool_T pool;

ForumCreate::~ForumCreate()
{
    beingDeleted();
}

void ForumCreate::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;

    hR.handlePostParams(request);
    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;

    PreparedStatement_T p = Connection_prepareStatement(con,
        "INSERT INTO Forums (name, short_name, user,user_id) VALUES (?, ?, ?,?)");
    const std::string name =hR.objectRequest["name"].toString().toStdString();
    PreparedStatement_setString(p, 1, name.c_str());
    const std::string short_name =hR.objectRequest["short_name"].toString().toStdString();

    PreparedStatement_setString(p, 2, short_name.c_str());
    const std::string user =hR.objectRequest["user"].toString().toStdString();

    PreparedStatement_setString(p, 3, user.c_str());
    int user_id = UserInfo::getUserID(user);
    PreparedStatement_setInt(p, 4, user_id);
    TRY
    {
        PreparedStatement_execute(p);
    }
    CATCH(SQLException)
    {
        ok = false;
    }
    END_TRY;
    Connection_close(con);

    hR.handleResponse();
    //проверка на код и всё такое тут должны быть
    hR.objectResponce["code"] = ok ? 0 : 5;
    bool isForumExist = true;
    hR.objectResponce["response"] = ForumInfo::getForumCreateInfo(hR.objectRequest["short_name"].toString(), isForumExist);

    hR.prepareOutput();

    response.setStatus(200);

    response.out() << hR.output;
}

ForumDetails::~ForumDetails()
{
    beingDeleted();
}

void ForumDetails::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    QString related;
    related = related.fromStdString(request.getParameter("related") ? *request.getParameter("related") : " ");
    QString short_name;
    short_name = short_name.fromStdString(request.getParameter("forum") ? *request.getParameter("forum") : " ");

    // QString user;

    bool isForumExist = false;

    hR.handleResponse();
    if (related != " ") {
        hR.responseContent = ForumInfo::getFullForumInfo(short_name, isForumExist);
    } else {
        hR.responseContent = ForumInfo::getForumCreateInfo(short_name, isForumExist);
    }

    hR.objectResponce["response"] = hR.responseContent;

    hR.objectResponce["code"] = isForumExist ? 0 : 1;

    hR.prepareOutput();
    response.setStatus(200);

    response.out() << hR.output;
}

ForumListPosts::~ForumListPosts()
{
    beingDeleted();
}

void ForumListPosts::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    QString forum;
    forum = forum.fromStdString(request.getParameter("forum") ? *request.getParameter("forum") : " ");

    QString order;
    order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : " ");
    //0 - magic constant for empty parametr
    QString since_id;
    since_id = since_id.fromStdString(request.getParameter("since") ? *request.getParameter("since") : " ");
    QString limit;
    limit = limit.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : " ");

    QString relatedArray[3]; //0-user, 1-forum,3 -thread

    auto temp = request.getParameterValues("related");

    for (auto i = temp.begin(); i != temp.end(); i++) {
        if ((*i) == "user")
            relatedArray[0] = "user";
        else if ((*i) == "forum")
            relatedArray[1] = "forum";
        else
            relatedArray[2] = "thread";
    }

    QString str_since = " ";
    QString str_limit = " ";
    QString str_order = " ";
    QString quote = "\"";

    if (since_id != " ")
        str_since = " AND p.date >= " + quote + since_id + quote;

    if (limit != " ")
        str_limit = " LIMIT " + limit;
    if (order == "asc")
        str_order = " ORDER BY p.date asc";
    else
        str_order = " ORDER BY p.date  desc";

    hR.handleResponse();
    QJsonArray arrayOfPosts;
    QString expression = "SELECT id, user, message,forum,thread_id, parent, date,likes, dislikes,isApproved,isHighlighted,isEdited,isSpam,isDeleted FROM Posts p WHERE p.forum=" + quote + forum + quote + str_since + str_order + str_limit + ";";
    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;
    ResultSet_T result;

    TRY
    {
        result = Connection_executeQuery(con, expression.toStdString().c_str());
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr<<"smth is wrong";
    }
    END_TRY;

    TRY
    {
        while (ResultSet_next(result)) {
            QString strGoodReply = Source::getPostTemplate();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
            QJsonObject jsonArray = jsonResponse.object();

            //   QJsonObject jsonArray;
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

            bool isUserExist = true; // костыль
            if (relatedArray[0] != "") {
                jsonArray["user"] = UserInfo::getFullUserInfo(jsonArray["user"].toString(), isUserExist);
            }
            if (relatedArray[1] != "") { //TODO
                jsonArray["forum"] = ForumInfo::getForumCreateInfo(forum, isUserExist);
            }
            if (relatedArray[2] != "") { //TODO
                jsonArray["thread"] = ThreadInfo::getFullThreadInfo(jsonArray["thread"].toInt(), isUserExist);
            }
            arrayOfPosts << jsonArray;
        }
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "smth is wrong :c";
    }
    END_TRY;
    Connection_close(con);
    hR.objectResponce["response"] = arrayOfPosts;

    hR.objectResponce["code"] = ok ? 0 : 1;

    hR.prepareOutput();
    response.setStatus(200);

    response.out() << hR.output;
}

ForumListThreads::~ForumListThreads()
{
    beingDeleted();
}

void ForumListThreads::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestList hR;
    QString forum;
    forum = forum.fromStdString(request.getParameter("forum") ? *request.getParameter("forum") : "");

    QString order;
    order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : "");
    //0 - magic constant for empty parametr
    QString since_id;
    since_id = since_id.fromStdString(request.getParameter("since") ? *request.getParameter("since") : "");
    QString limit;
    limit = limit.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : "");

    QString relatedArray[2]; //0-user, 1-forum
    relatedArray[0] = "";
    relatedArray[1] = "";

    auto temp = request.getParameterValues("related");

    for (auto i = temp.begin(); i != temp.end(); i++) {
        if ((*i) == "user")
            relatedArray[0] = "user";
        else if ((*i) == "forum")
            relatedArray[1] = "forum";
    }

    QString str_since = " ";
    QString str_limit = " ";
    QString str_order = " ";
    QString quote = "\"";

    if (since_id != "")
        str_since = " AND p.date >= " + quote + since_id + quote;

    if (limit != "")
        str_limit = " LIMIT " + limit;
    if (order == "asc")
        str_order = " ORDER BY p.date asc";
    else
        str_order = " ORDER BY p.date  desc";

    hR.handleResponse();
    QJsonArray arrayOfPosts;
    QString expression = "SELECT id,forum,user,title,slug,message,date,likes,dislikes,isClosed,isDeleted FROM Threads p WHERE p.forum=" + quote + forum + quote + str_since + str_order + str_limit + ";";
    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;
    ResultSet_T result;

    TRY
    {
        result = Connection_executeQuery(con, expression.toStdString().c_str());
    }
    CATCH(SQLException)
    {
        ok = false;
    }
    END_TRY;

    TRY
    {
        while (ResultSet_next(result)) {
            QString strGoodReply = Source::getFullThreadTemplate();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
            QJsonObject jsonArray = jsonResponse.object();

            jsonArray["id"] = ResultSet_getInt(result, 1);
            jsonArray["forum"] = ResultSet_getString(result, 2);
            jsonArray["user"] = ResultSet_getString(result, 3);
            jsonArray["title"] = ResultSet_getString(result, 4);
            jsonArray["slug"] = ResultSet_getString(result, 5);
            jsonArray["message"] = ResultSet_getString(result, 6);

            auto date = ResultSet_getDateTime(result, 7);
            jsonArray["date"] = Convertor::getTime(date);

            jsonArray["likes"] = ResultSet_getInt(result, 8);
            jsonArray["dislikes"] = ResultSet_getInt(result, 9);
            jsonArray["isClosed"] = ResultSet_getInt(result, 10);
            jsonArray["isDeleted"] = ResultSet_getInt(result, 11);
            jsonArray["points"] = ResultSet_getInt(result, 8) - ResultSet_getInt(result, 9);
            jsonArray["posts"] = PostInfo::countPosts(jsonArray["id"].toInt());

            bool isUserExist = true; // костыль
            if (relatedArray[0] != "") {
                jsonArray["user"] = UserInfo::getFullUserInfo(jsonArray["user"].toString(), isUserExist);
            }
            if (relatedArray[1] != "") {
                jsonArray["forum"] = ForumInfo::getForumCreateInfo(forum, isUserExist);
            }

            arrayOfPosts << jsonArray;
        }
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "smth is wrong :c";
    }
    END_TRY;
    Connection_close(con);

    hR.objectResponce["response"] = arrayOfPosts;

    hR.objectResponce["code"] = ok ? 0 : 1;

    hR.prepareOutput();
    response.setStatus(200);

    response.out() << hR.output;
}

ForumListUsers::~ForumListUsers()
{
    beingDeleted();
}

void ForumListUsers::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestList hR;
    QString forum;
    forum = forum.fromStdString(request.getParameter("forum") ? *request.getParameter("forum") : " ");

    QString order;
    order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : " ");
    //0 - magic constant for empty parametr
    QString since_id;
    since_id = forum.fromStdString(request.getParameter("since_id") ? *request.getParameter("since_id") : " ");
    QString limit;
    limit = forum.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : " ");

    QString str_since;
    QString str_limit;
    QString str_order;
    QString quote = "\"";

    if (since_id != " ")
        str_since = " AND u.id >= " + quote + since_id + quote;

    if (limit != " ")
        str_limit = " LIMIT " + limit;
    if (order == "asc")
        str_order = " ORDER BY u.name asc ";
    else
        str_order = " ORDER BY u.name desc ";

    QString expression;
    expression = "SELECT   u.id, u.email,u.username, u.about, u.name, u.isAnonymous FROM Users u WHERE id IN (SELECT DISTINCT user_id FROM Posts p WHERE p.forum=" + quote + forum + quote +")" +str_since + str_order + str_limit + ";";

    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;
    ResultSet_T result;
    hR.handleResponse();
    QJsonArray arrayOfThreads;

    TRY
    {
        result = Connection_executeQuery(con, expression.toStdString().c_str());
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr<<"LISTUSERS ERROR";
    }
    END_TRY;

    TRY
    {
        while (ResultSet_next(result)) {

            QString strGoodReply = Source::getUserTemplate();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(strGoodReply.toUtf8());
            //  QJsonObject hR.objectResponce = jsonResponse.object();
            QJsonObject jsonArray = jsonResponse.object();
            jsonArray["id"] = ResultSet_getInt(result, 1);
            jsonArray["email"] = ResultSet_getString(result, 2);
            std::string temp= ResultSet_getString(result, 3); //¯\_(ツ)_/¯
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

            UserInsideInfo userInsideInfo;

            QJsonArray followers = userInsideInfo.getFollowers(jsonArray["email"].toString());
            QJsonArray followee = userInsideInfo.getFollowee(jsonArray["email"].toString());
            jsonArray["following"] = followee;
            jsonArray["followers"] = followers;

            QJsonArray subscriptions = userInsideInfo.getSubscriptions(jsonArray["email"].toString());
            jsonArray["subscriptions"] = subscriptions;

            arrayOfThreads << jsonArray;
        }
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "smth is wrong :c";
    }
    END_TRY;
    Connection_close(con);
    hR.objectResponce["response"] = arrayOfThreads;
    hR.prepareOutput();

    response.setStatus(200);

    response.out() << hR.output;
}
