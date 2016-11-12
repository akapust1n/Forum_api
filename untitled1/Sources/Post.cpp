#include "Post.h"
#include <zdb/zdb.h>

extern ConnectionPool_T pool;
PostCreate::~PostCreate()
{
    beingDeleted();
}

void PostCreate::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    if (hR.objectRequest["isDeleted"] == "")
        hR.objectRequest["isDeleted"] = false;

    if (hR.objectRequest["parent"] == "")
        hR.objectRequest["parent"] = false;

    if (hR.objectRequest["isApproved"] == "")
        hR.objectRequest["isApproved"] = false;

    if (hR.objectRequest["isHighlighted"] == "")
        hR.objectRequest["isHighlighted"] = false;

    if (hR.objectRequest["isEdited"] == "")
        hR.objectRequest["isEdited"] = true;

    if (hR.objectRequest["isSpam"] == "")
        hR.objectRequest["isSpam"] = false;
    QString path;
    if (hR.objectRequest["parent"] == QJsonValue::Null)
        path = "";
    Connection_T con = ConnectionPool_getConnection(pool);
    Connection_beginTransaction(con);
    bool ok = true;
    PreparedStatement_T p = Connection_prepareStatement(con,
        "INSERT INTO Posts (date, thread_id, message, user, forum, parent, isApproved, isHighlighted, isEdited, isSpam, isDeleted, path) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");

    const std::string date = hR.objectRequest["date"].toString().toStdString();
    PreparedStatement_setString(p, 1, date.c_str());

    PreparedStatement_setInt(p, 2, hR.objectRequest["thread"].toInt());

    const std::string message = hR.objectRequest["message"].toString().toStdString();
    PreparedStatement_setString(p, 3, message.c_str());

    const std::string user = hR.objectRequest["user"].toString().toStdString();
    PreparedStatement_setString(p, 4, user.c_str());

    const std::string forum = hR.objectRequest["forum"].toString().toStdString();
    PreparedStatement_setString(p, 5, forum.c_str());

    if (hR.objectRequest["parent"] != QJsonValue::Null) {
        PreparedStatement_setInt(p, 6, hR.objectRequest["parent"].toInt());
        path = PostInfo::getPath(hR.objectRequest["parent"].toInt());

    } else {
        path = PostInfo::getPath(-1);
        const char* nll = NULL;
        PreparedStatement_setString(p, 6, nll);
    }
    PreparedStatement_setInt(p, 7, hR.objectRequest["isApproved"].toBool());
    PreparedStatement_setInt(p, 8, hR.objectRequest["isHighlighted"].toBool());
    PreparedStatement_setInt(p, 9, hR.objectRequest["isEdited"].toBool());
    PreparedStatement_setInt(p, 10, hR.objectRequest["isSpam"].toBool());
    PreparedStatement_setInt(p, 11, hR.objectRequest["isDeleted"].toBool());

    const std::string _path = path.toStdString();

    PreparedStatement_setString(p, 12, _path.c_str());

    hR.handleResponse();
    int lastInsertID = 0;
    TRY
    {
        PreparedStatement_execute(p);
        ResultSet_T result;
        result = Connection_executeQuery(con, "SELECT LAST_INSERT_ID()");
        while (ResultSet_next(result))
            lastInsertID = ResultSet_getInt(result, 1);
        Connection_commit(con);
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "POSTCREATE ERROR";
    }
    END_TRY;

    hR.objectResponce["code"] = ok ? 0 : 4;
    bool isThreadExist = true;
    auto tt = Connection_lastRowId(con);
    auto tt1 = Connection_rowsChanged(con);
    auto tt2 = PreparedStatement_rowsChanged(p);
    hR.objectResponce["response"] = PostInfo::getFullPostInfo(lastInsertID, isThreadExist);

    Connection_close(con);

    hR.prepareOutput();
    response.setStatus(200);

    response.out() << hR.output;

    // std::cout << hR.output << "Tat";
}

PostDetails::~PostDetails()
{
    beingDeleted();
}

void PostDetails::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    QString post;
    post = post.fromStdString(request.getParameter("post") ? *request.getParameter("post") : " ");
    int post_id = post.toInt();

    QString relatedArray[3]; //0-user, 1-forum,3 -thread
    relatedArray[0] = " ";
    relatedArray[1] = " ";
    relatedArray[2] = " ";
    auto temp = request.getParameterValues("related");

    for (auto i = temp.begin(); i != temp.end(); i++) {
        if ((*i) == "user")
            relatedArray[0] = "user";
        else if ((*i) == "forum")
            relatedArray[1] = "forum";
        else
            relatedArray[2] = "thread";
    }

    bool isPostExist = false;

    hR.handleResponse();
    hR.responseContent = PostInfo::getFullPostInfo(post_id, isPostExist);

    //  isUserExist костыль, надо как-то перерабыватывать функцию
    if (relatedArray[0] != " ") {
        hR.responseContent["user"] = UserInfo::getFullUserInfo(hR.responseContent["user"].toString(), isPostExist);
    }
    if (relatedArray[1] != " ") { //TODO
        hR.responseContent["forum"] = ForumInfo::getFullForumInfo(hR.responseContent["forum"].toString(), isPostExist);
    }
    if (relatedArray[2] != " ") { //TODO
        hR.responseContent["thread"] = ThreadInfo::getFullThreadInfo(hR.responseContent["thread"].toInt(), isPostExist);
    }

    hR.objectResponce["response"] = hR.responseContent;

    hR.objectResponce["code"] = isPostExist ? 0 : 1;

    hR.prepareOutput();
    response.setStatus(200);

    response.out() << hR.output;
}

PostRemove::~PostRemove()
{
    beingDeleted();
}

void PostRemove::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    hR.handleResponse();

    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;

    PreparedStatement_T p = Connection_prepareStatement(con,
        "UPDATE Posts SET isDeleted=true WHERE id=?");
    PreparedStatement_setInt(p, 1, hR.objectRequest["post"].toInt());
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

    hR.responseContent["post"] = hR.objectRequest["post"];
    hR.objectResponce["code"] = ok ? 0 : 1;
    hR.objectResponce["response"] = hR.responseContent;

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
}

PostRestore::~PostRestore()
{
    beingDeleted();
}

void PostRestore::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    hR.handleResponse();

    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;

    PreparedStatement_T p = Connection_prepareStatement(con,
        "UPDATE Posts SET isDeleted=false WHERE id=?");
    PreparedStatement_setInt(p, 1, hR.objectRequest["post"].toInt());
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

    hR.responseContent["post"] = hR.objectRequest["post"];
    hR.objectResponce["code"] = ok ? 0 : 1;
    hR.objectResponce["response"] = hR.responseContent;

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
}

PostUpdate::~PostUpdate()
{
    beingDeleted();
}

void PostUpdate::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    hR.handleResponse();

    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;

    PreparedStatement_T p = Connection_prepareStatement(con,
        "UPDATE Posts SET message=? WHERE id=?");
    const std::string message = hR.objectRequest["message"].toString().toStdString();
    PreparedStatement_setString(p, 1, message.c_str());
    PreparedStatement_setInt(p, 2, hR.objectRequest["post"].toInt());
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

    hR.responseContent["post"] = hR.objectRequest["post"];
    hR.objectResponce["code"] = ok ? 0 : 1;
    hR.objectResponce["response"] = hR.responseContent;

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
}

PostVote::~PostVote()
{
    beingDeleted();
}

void PostVote::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{

    HandleRequestBase hR;
    hR.handlePostParams(request);
    hR.handleResponse();

    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;

    PreparedStatement_T p = Connection_prepareStatement(con,
        "UPDATE Posts SET likes=likes+?, dislikes=dislikes+? WHERE id =?");
    PreparedStatement_setInt(p, 1, (hR.objectRequest["vote"].toInt() > 0 ? 1 : 0));
    PreparedStatement_setInt(p, 2, (hR.objectRequest["vote"].toInt() > 0 ? 0 : 1));
    PreparedStatement_setInt(p, 3, hR.objectRequest["post"].toInt());
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

    hR.responseContent["post"] = hR.objectRequest["post"];
    hR.objectResponce["code"] = ok ? 0 : 1;
    hR.objectResponce["response"] = hR.responseContent;

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
}

PostList::~PostList()
{
    beingDeleted();
}

void PostList::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestList hR;
    QString threadOrForum;
    bool isForum = false;
    threadOrForum = threadOrForum.fromStdString(request.getParameter("thread") ? *request.getParameter("thread") : "");
    if (threadOrForum == "") {
        isForum = true;
        threadOrForum = threadOrForum.fromStdString(request.getParameter("forum") ? *request.getParameter("forum") : "");
    }
    QString order;
    order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : "desc");
    //0 - magic constant for empty parametr
    QString since_id;
    since_id = threadOrForum.fromStdString(request.getParameter("since") ? *request.getParameter("since") : "");
    QString limit;
    limit = threadOrForum.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : "");

    QString str_since;
    QString str_limit;
    QString str_order;
    QString quote = "\"";

    if (since_id != "")
        str_since = " AND p.date >= " + quote + since_id + quote;

    if (limit != "")
        str_limit = " LIMIT " + limit;
    if (order != "")
        str_order = " ORDER BY p.date " + order;

    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;
    QString expression;
    if (!isForum) {
        expression = "SELECT p.id, p.date, p.dislikes, p.forum, p.dislikes, p.forum, p.id, p.isApproved, p.isDeleted, p.isEdited, p.isHighlighted, p.isSpam, p.likes, p.message, p.thread_id, p.user, p.parent, p.likes-p.dislikes as points FROM Posts p JOIN Threads second ON second.id=p.thread_id WHERE second.id=" + threadOrForum + str_since + str_order + str_limit + ";";

    } else {
        expression = "SELECT p.id,p.date, p.dislikes, p.forum, p.dislikes, p.forum, p.id, p.isApproved, p.isApproved, p.isDeleted, p.isEdited, p.isHighlighted, p.isSpam, p.likes, p.message, p.thread_id, p.user, p.parent, p.likes-p.dislikes as points FROM Posts p JOIN Forums second ON p.forum=second.short_name WHERE second.short_name=" + quote + threadOrForum + quote + str_since + str_order + str_limit + ";";
    }
    ResultSet_T result;

    TRY
    {
        result = Connection_executeQuery(con, expression.toStdString().c_str());
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "smth is wrong";
    }
    END_TRY;

    hR.handleResponse();
    QJsonArray arrayOfPosts;
    bool isUserExist = true; // заглушка

    TRY
    {
        while (ResultSet_next(result)) {
            {
                int id = ResultSet_getInt(result, 1);
                QJsonObject jsonObj = PostInfo::getFullPostInfo(id, isUserExist); // assume this has been populated with Json data
                arrayOfPosts << jsonObj;
            }
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

    hR.prepareOutput();

    response.setStatus(200);

    response.out() << hR.output;
}
