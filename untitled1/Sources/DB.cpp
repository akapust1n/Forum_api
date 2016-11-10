#include <DB.h>

extern ConnectionPool_T pool;
DbClear::~DbClear()
{
    beingDeleted();
}

void DbClear::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
    Connection_T con = ConnectionPool_getConnection(pool);

    std::string qStr1 = "TRUNCATE TABLE Users;";
    std::string qStr2 = "TRUNCATE TABLE Forums;";
    std::string qStr3 = "TRUNCATE TABLE Threads;";
    std::string qStr4 = "TRUNCATE TABLE Posts;";
    std::string qStr5 = "TRUNCATE TABLE Followers;";
    std::string qStr6 = "TRUNCATE TABLE Subscribers;";
    std::string turn_off = "SET foreign_key_checks = 0;";
    std::string turn_on = "SET foreign_key_checks = 1;";

    bool rollback = false;
    TRY
    {
        Connection_beginTransaction(con);
        Connection_execute(con, turn_off.c_str());
        Connection_execute(con, qStr1.c_str());
        Connection_execute(con, qStr2.c_str());
        Connection_execute(con, qStr3.c_str());
        Connection_execute(con, qStr4.c_str());
        Connection_execute(con, qStr5.c_str());
        Connection_execute(con, qStr6.c_str());
        Connection_execute(con, turn_on.c_str());
    }
    CATCH(SQLException)
    {
        Connection_rollback(con);
        rollback = true;
    }
    END_TRY;
    Connection_close(con);
    if (!rollback)
        Connection_commit(con);

    Wt::Json::Object data;

    data["code"] = Wt::Json::Value(0);
    data["response"] = Wt::Json::Value("OK");
    response.setStatus(200);
    response.setMimeType("application/json");

    response.out() << serialize(data);

    //response.out() << "Db clear!\n";
}

DbStatus::~DbStatus()
{
    beingDeleted();
}

void DbStatus::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{

    Wt::Json::Object data;

    Wt::Json::parse("{ "
                    "  \"code\": 0, "
                    "  \"response\":{\"user\": 100000, \"thread\": 1000, \"forum\": 100, \"post\": 1000000} "
                    "}",
        data);

    response.setStatus(200);
    response.setMimeType("application/json");
    response.out() << serialize(data);
}
