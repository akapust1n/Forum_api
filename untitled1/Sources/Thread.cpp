#include "Thread.h"
#include <zdb/zdb.h>

extern ConnectionPool_T pool;

ThreadCreate::~ThreadCreate()
{
    beingDeleted();
}

void ThreadCreate::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    if (hR.objectRequest["isDeleted"].toString().length() == 0)
        hR.objectRequest["isDeleted"] = false;
    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T p = Connection_prepareStatement(con,
        "INSERT INTO Threads (forum, title, isClosed, user, date, message, slug, isDeleted) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

    const std::string forum = hR.objectRequest["forum"].toString().toStdString();
    PreparedStatement_setString(p, 1, forum.c_str());

    const std::string title = hR.objectRequest["title"].toString().toStdString();
    PreparedStatement_setString(p, 2, title.c_str());

    PreparedStatement_setInt(p, 3, hR.objectRequest["isClosed"].toBool());

    const std::string user = hR.objectRequest["user"].toString().toStdString();
    PreparedStatement_setString(p, 4, user.c_str());

    const std::string data = hR.objectRequest["date"].toString().toStdString();
    PreparedStatement_setString(p, 5, data.c_str());

    const std::string message = hR.objectRequest["message"].toString().toStdString();
    PreparedStatement_setString(p, 6, message.c_str());

    const std::string slug = hR.objectRequest["slug"].toString().toStdString();
    PreparedStatement_setString(p, 7, slug.c_str());

    PreparedStatement_setInt(p, 8, hR.objectRequest["isDeleted"].toBool());
    bool ok = true;

    hR.handleResponse();
    TRY
    {
        PreparedStatement_execute(p);
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "THREAD DOESNT CREATE";
    }
    END_TRY;

    if (ok) {
        hR.objectResponce["code"] = 0;
        bool isThreadExist = true; //костыль(
        ResultSet_T result;
        int lastId;
        TRY
        {
            result = Connection_executeQuery(con, "Select count(*) from Threads");
            while (ResultSet_next(result)) {
                lastId = ResultSet_getInt(result, 1);
            }
        }
        CATCH(SQLException)
        {
            ok = false;
            std::cerr << "smth is  wrong";
        }
        END_TRY;
        hR.objectResponce["response"] = ThreadInfo::getThreadCreateInfo(lastId, isThreadExist);
    }

    else {
        hR.objectResponce["code"] = 4;
    }
    Connection_close(con);

    hR.prepareOutput();
    response.setStatus(200);

    response.out() << hR.output;
}

ThreadDetails::~ThreadDetails()
{
    beingDeleted();
}

void ThreadDetails::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    //ПЕРЕПИСАТЬ В USERINFO ВСЕ ЗАПРОСЫ ПРО ЮЗЕРА ((
    //на данный момент это просто захардкожено для прикола
    //Возможно, уже переписал
    HandleRequestBase hR;
    QString thread;
    thread = thread.fromStdString(request.getParameter("thread") ? *request.getParameter("thread") : "");
    int thread_id = thread.toInt();
    QString relatedArray[2]; //0-user, 1-forum
    relatedArray[0] = " ";
    relatedArray[1] = " ";
    bool error = false;

    auto temp = request.getParameterValues("related");

    for (auto i = temp.begin(); i != temp.end(); i++) {
        if ((*i) == "user")
            relatedArray[0] = "user";
        else if ((*i) == "forum")
            relatedArray[1] = "forum";
        else
            error = true;
    }

    bool isThreadExist = false;

    hR.handleResponse();
    hR.responseContent = ThreadInfo::getFullThreadInfo(thread_id, isThreadExist);

    //  isUserExist костыль, надо как-то перерабыватывать функцию
    if (relatedArray[0] != " ") {
        hR.responseContent["user"] = UserInfo::getFullUserInfo(hR.responseContent["user"].toString(), isThreadExist);
    }
    if (relatedArray[1] != " ") { //TODO
        // hR.responseContent["user"] = UserInfo::getFullUserInfo(hR.responseContent["user"], isUserExist);
    }
    if (hR.responseContent["isDeleted"].toBool()) {
        hR.responseContent["posts"] = 0;
    }
    hR.objectResponce["response"] = hR.responseContent;

    hR.objectResponce["code"] = isThreadExist ? 0 : 1;
    if (error)
        hR.objectResponce["code"] = 3;

    hR.prepareOutput();
    response.setStatus(200);

    response.out() << hR.output;
}

ThreadClose::~ThreadClose()
{
    beingDeleted();
}

void ThreadClose::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;
    hR.handlePostParams(request);
    hR.handleResponse();
    PreparedStatement_T p = Connection_prepareStatement(con,
        "UPDATE Threads SET isClosed=true WHERE id=?;");
    PreparedStatement_setInt(p, 1, hR.objectRequest["thread"].toInt());
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

    hR.responseContent["thread"] = hR.objectRequest["thread"];
    hR.objectResponce["code"] = ok ? 0 : 1;
    hR.objectResponce["response"] = hR.responseContent;

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
}

ThreadOpen::~ThreadOpen()
{
    beingDeleted();
}

void ThreadOpen::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    hR.handleResponse();
    Connection_T con = ConnectionPool_getConnection(pool);
    PreparedStatement_T p = Connection_prepareStatement(con, "UPDATE Threads SET isClosed=false WHERE id=?;");
    PreparedStatement_setInt(p, 1, hR.objectRequest["thread"].toInt());
    bool ok = true;
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
    hR.responseContent["thread"] = hR.objectRequest["thread"];
    hR.objectResponce["code"] = ok ? 0 : 1;
    hR.objectResponce["response"] = hR.responseContent;

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
}

ThreadRemove::~ThreadRemove()
{
    beingDeleted();
}

void ThreadRemove::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    hR.handleResponse();
    Connection_T con = ConnectionPool_getConnection(pool);

    PreparedStatement_T p = Connection_prepareStatement(con, "UPDATE Threads SET isDeleted=true WHERE id=?;");
    PreparedStatement_setInt(p, 1, hR.objectRequest["thread"].toInt());
    Connection_beginTransaction(con);
    bool ok = true;
    TRY
    {
        PreparedStatement_execute(p);
    }
    CATCH(SQLException)
    {
        Connection_rollback(con);
        ok = false;
        std::cerr << "smth is wrong";
    }
    END_TRY;
    p = Connection_prepareStatement(con, "UPDATE Posts SET isDeleted=true WHERE thread_id=?;");
    PreparedStatement_setInt(p, 1, hR.objectRequest["thread"].toInt());

    TRY
    {
        PreparedStatement_execute(p);
        Connection_commit(con);
    }
    CATCH(SQLException)
    {
        Connection_rollback(con);
        ok = false;
        std::cerr << "smth is wrong";
    }
    END_TRY;
    Connection_close(con);

    hR.responseContent["thread"] = hR.objectRequest["thread"];

    hR.objectResponce["code"] = ok ? 0 : 1;
    hR.objectResponce["response"] = hR.responseContent;

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
}

ThreadRestore::~ThreadRestore()
{
    beingDeleted();
}

void ThreadRestore::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    hR.handleResponse();
    Connection_T con = ConnectionPool_getConnection(pool);

    PreparedStatement_T p = Connection_prepareStatement(con, "UPDATE Threads SET isDeleted=false WHERE id=?;");
    PreparedStatement_setInt(p, 1, hR.objectRequest["thread"].toInt());
    Connection_beginTransaction(con);
    bool ok = true;
    TRY
    {
        PreparedStatement_execute(p);
    }
    CATCH(SQLException)
    {
        Connection_rollback(con);
        ok = false;
        std::cerr << "smth is wrong";
    }
    END_TRY;

    p = Connection_prepareStatement(con, "UPDATE Posts SET isDeleted=false WHERE thread_id=?;");
    PreparedStatement_setInt(p, 1, hR.objectRequest["thread"].toInt());

    TRY
    {
        PreparedStatement_execute(p);
        Connection_commit(con);
    }
    CATCH(SQLException)
    {
        Connection_rollback(con);
        ok = false;
        std::cerr << "smth is wrong";
    }
    END_TRY;
    Connection_close(con);

    hR.responseContent["thread"] = hR.objectRequest["thread"];
    hR.objectResponce["code"] = ok ? 0 : 1;
    hR.objectResponce["response"] = hR.responseContent;

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
}

ThreadUpdate::~ThreadUpdate()
{
    beingDeleted();
}

void ThreadUpdate::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    hR.handleResponse();
    Connection_T con = ConnectionPool_getConnection(pool);
    bool ok = true;

    PreparedStatement_T p = Connection_prepareStatement(con, "UPDATE Threads SET message=?, slug=? WHERE id=?;");
    const std::string message = hR.objectRequest["message"].toString().toStdString();
    const std::string slug = hR.objectRequest["slug"].toString().toStdString();

    PreparedStatement_setString(p, 1, message.c_str());
    PreparedStatement_setString(p, 2, slug.c_str());
    PreparedStatement_setInt(p, 3, hR.objectRequest["thread"].toInt());
    TRY
    {
        PreparedStatement_execute(p);
    }
    CATCH(SQLException)
    {
        Connection_rollback(con);
        ok = false;
        std::cerr << "smth is wrong";
    }
    END_TRY;
    Connection_close(con);

    hR.objectResponce["code"] = ok ? 0 : 1;
    bool tt = true;
    hR.objectResponce["response"] = ThreadInfo::getFullThreadInfo(hR.objectRequest["thread"].toInt(), tt);

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
}

ThreadVote::~ThreadVote()
{
    beingDeleted();
}

void ThreadVote::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    hR.handleResponse();
    Connection_T con = ConnectionPool_getConnection(pool);

    PreparedStatement_T p = Connection_prepareStatement(con, "UPDATE Threads SET likes=likes+?, dislikes=dislikes+? WHERE id=?");
    PreparedStatement_setInt(p, 1, hR.objectRequest["vote"].toInt() > 0);
    PreparedStatement_setInt(p, 2, hR.objectRequest["vote"].toInt() < 0);
    PreparedStatement_setInt(p, 3, hR.objectRequest["thread"].toInt());
    bool ok = true;
    TRY
    {
        PreparedStatement_execute(p);
    }
    CATCH(SQLException)
    {
        Connection_rollback(con);
        ok = false;
        std::cerr << "smth is wrong";
    }
    END_TRY;
    Connection_close(con);

    hR.objectResponce["code"] = ok ? 0 : 1;
    bool tt = true;
    hR.objectResponce["response"] = ThreadInfo::getFullThreadInfo(hR.objectRequest["thread"].toInt(), tt);
    ;

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
}

ThreadSubscribe::~ThreadSubscribe()
{
    beingDeleted();
}

void ThreadSubscribe::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    hR.handleResponse();
    Connection_T con = ConnectionPool_getConnection(pool);

    PreparedStatement_T p = Connection_prepareStatement(con, "INSERT INTO Subscribers (user, thread_id) VALUES (?, ?);");
    const std::string user = hR.objectRequest["user"].toString().toStdString();
    PreparedStatement_setString(p, 1, user.c_str());
    PreparedStatement_setInt(p, 2, hR.objectRequest["thread"].toInt());

    bool ok = true;
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

    hR.responseContent["thread"] = hR.objectRequest["thread"];
    hR.responseContent["user"] = hR.objectRequest["user"];

    hR.objectResponce["code"] = ok ? 0 : 1;
    hR.objectResponce["response"] = hR.responseContent;

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
}

ThreadUnSubscribe::~ThreadUnSubscribe()
{
    beingDeleted();
}

void ThreadUnSubscribe::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestBase hR;
    hR.handlePostParams(request);
    hR.handleResponse();
    Connection_T con = ConnectionPool_getConnection(pool);

    PreparedStatement_T p = Connection_prepareStatement(con, "DELETE FROM Subscribers WHERE user=? AND thread_id=?;");
    const std::string user = hR.objectRequest["user"].toString().toStdString();

    PreparedStatement_setString(p, 1, user.c_str());
    PreparedStatement_setInt(p, 2, hR.objectRequest["thread"].toInt());
    bool ok = true;
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

    hR.responseContent["thread"] = hR.objectRequest["thread"];
    hR.responseContent["user"] = hR.objectRequest["user"];

    hR.objectResponce["code"] = ok ? 0 : 1;
    hR.objectResponce["response"] = hR.responseContent;

    hR.prepareOutput();
    response.setStatus(200);
    response.out() << hR.output;
//    std::cout << hR.output << " output";
}

ThreadList::~ThreadList()
{
    beingDeleted();
}

void ThreadList::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestList hR;
    QString userOrForum;
    bool isForum = false;
    userOrForum = userOrForum.fromStdString(request.getParameter("user") ? *request.getParameter("user") : " ");
    if (userOrForum == " ") {
        isForum = true;
        userOrForum = userOrForum.fromStdString(request.getParameter("forum") ? *request.getParameter("forum") : " ");
    }
    QString order;
    order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : " ");
    //0 - magic constant for empty parametr
    QString since_id;
    since_id = userOrForum.fromStdString(request.getParameter("since") ? *request.getParameter("since") : " ");
    QString limit;
    limit = userOrForum.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : " ");

    QString str_since;
    QString str_limit;
    QString str_order;
    QString quote = "\"";

    if (since_id != " ")
        str_since = " AND date >= " + quote + since_id + quote;

    if (limit != " ")
        str_limit = " LIMIT " + limit;
    if (order == "asc")
        str_order = " ORDER BY date asc ";
    else
        str_order = " ORDER BY date desc ";
    QString expression;
    if (!isForum) {
        expression = "SELECT * FROM Threads WHERE user=" + quote + userOrForum + quote + str_since + str_order + str_limit + ";";

    } else {
        expression = "SELECT * FROM Threads WHERE forum=" + quote + userOrForum + quote + str_since + str_order + str_limit + ";";
    }
    Connection_T con = ConnectionPool_getConnection(pool);

    ResultSet_T result;
    hR.handleResponse();
    QJsonArray arrayOfThreads;
    bool isThreadExist = true; // заглушка
    bool ok = true;
    TRY
    {
        result = Connection_executeQuery(con, expression.toStdString().c_str());
        while (ResultSet_next(result)) {
            int id = ResultSet_getInt(result, 1);
            QJsonObject jsonObj = ThreadInfo::getFullThreadInfo(id, isThreadExist); // assume this has been populated with Json data
            arrayOfThreads << jsonObj;
        }
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "smth is wrong";
    }
    END_TRY;
    Connection_close(con);

    hR.objectResponce["response"] = arrayOfThreads;

    hR.prepareOutput();

    response.setStatus(200);

    response.out() << hR.output;
}

ThreadListPost::~ThreadListPost()
{
    beingDeleted();
}

void ThreadListPost::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    HandleRequestList hR;
    QString thread;
    thread = thread.fromStdString(request.getParameter("thread") ? *request.getParameter("thread") : "");

    QString order;
    order = order.fromStdString(request.getParameter("order") ? *request.getParameter("order") : "");
    //0 - magic constant for empty parametr
    QString since_id;
    since_id = thread.fromStdString(request.getParameter("since") ? *request.getParameter("since") : "");
    QString limit;
    limit = thread.fromStdString(request.getParameter("limit") ? *request.getParameter("limit") : "");
    QString sort;
    sort = thread.fromStdString(request.getParameter("sort") ? *request.getParameter("sort") : "");

    QString str_since = " ";
    QString str_limit = " ";
    QString str_order = " ";
    QString str_sort = " ";
    QString quote = "\"";

    if (since_id != "")
        str_since = " AND date >= " + quote + since_id + quote;
    if (limit != "")
        str_limit = " LIMIT " + limit;

    if (sort == "flat" || sort == "")
        str_sort = " ORDER BY date ";
    str_order = " " + order;

    if (sort == "tree") {
        str_sort = "ORDER BY Pathlvl1 desc,Pathlvl2 asc , Pathlvl3 asc, Pathlvl4 asc ";
        if (order == "asc") {
            str_sort = " ORDER BY Pathlvl1,Pathlvl2, Pathlvl3,Pathlvl4";
        }
        str_order = " ";
    }
    Connection_T con = ConnectionPool_getConnection(pool);

    if (sort == "parent_tree") {
        if (order == "asc") {
            QString expression = "SELECT distinct Pathlvl1  from Posts p WHERE p.thread_id=" + quote + thread + quote + str_since +" ORDER BY Pathlvl1 asc " + str_limit+";";
            ResultSet_T result;
            bool ok = true;
            TRY
            {
                result = Connection_executeQuery(con, expression.toStdString().c_str());
                int max_value;
                while (ResultSet_next(result)) {
                     max_value = ResultSet_getInt(result, 1);
                }

                str_sort = " AND (Pathlvl1 <=" +QString::number(max_value)+ ")";
                str_limit = " ";
            }

            CATCH(SQLException)
            {
                ok = false;
                std::cerr << "wrong parent tree error";
            }
            END_TRY;
            str_order = " ORDER BY Pathlvl1 asc, Pathlvl2 asc, Pathlvl3 asc, Pathlvl4 asc ";

        }

        if (order == "desc" || order == " ") {
            QString expression = "SELECT distinct Pathlvl1 from Posts p WHERE p.thread_id=" + quote + thread + quote + str_since + str_limit+" ORDER BY Pathlvl1 desc;";
            ResultSet_T result;
            bool ok = true;
            TRY
            {
                result = Connection_executeQuery(con, expression.toStdString().c_str());
                int max_value;
                while (ResultSet_next(result)) {
                     max_value = ResultSet_getInt(result, 1);
                }

                str_sort = " AND (Pathlvl1 >=" +QString::number(max_value)+ ")";
                str_limit = " ";
            }

            CATCH(SQLException)
            {
                ok = false;
                std::cerr << "smth is wrong";
            }
            END_TRY;
            str_order = " ORDER BY Pathlvl1 desc, Pathlvl2 asc, Pathlvl3 asc, Pathlvl4 asc ";

        }
    }
    bool ok = true;
    QString expression;
    expression = "SELECT p.date, p.dislikes, p.forum, p.id, p.isApproved, p.isDeleted, p.isEdited, p.isHighlighted, p.isSpam, p.likes, p.message, p.thread_id, p.user, p.parent, p.likes-p.dislikes as points FROM Posts p WHERE p.thread_id=" + quote + thread + quote + str_since + str_sort + str_order + str_limit + ";";

    hR.handleResponse();
    QJsonArray arrayOfPosts;
    bool isPostExist = true; // заглушка
    ResultSet_T result;
    TRY
    {

        result = Connection_executeQuery(con, expression.toStdString().c_str());
        if(sort == "parent_tree")
            std::cout<<" error "<<Connection_getLastError(con);

        while (ResultSet_next(result)) {
            int id = ResultSet_getInt(result, 4);
            QJsonObject jsonObj = PostInfo::getFullPostInfo(id, isPostExist); // assume this has been populated with Json data
            arrayOfPosts << jsonObj;
        }
    }
    CATCH(SQLException)
    {
        ok = false;
        std::cerr << "ListPost errpr";
    }
    END_TRY;
    Connection_close(con);
    hR.objectResponce["code"] = ok ? 0 : 1;
    hR.objectResponce["response"] = arrayOfPosts;

    hR.prepareOutput();

    response.setStatus(200);

    response.out() << hR.output;
}
