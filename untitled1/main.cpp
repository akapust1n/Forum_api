#include <Wt/Http/Response>
#include <Wt/WResource>
#include <Wt/WServer>
#include <BdWrapper.h>
#include "DB.h"

using namespace Wt;

class RestGetHello : public Wt::WResource {
public:
    virtual ~RestGetHello()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        response.out() << "Hello Rest!\n";
    }
};
class RestGetBye : public Wt::WResource {
public:
    virtual ~RestGetBye()
    {
        beingDeleted();
    }

protected:
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
    {
        response.out() << "Bye Rest!\n";
    }
};

int main(int argc, char** argv)
{
    try {
        WServer server(argv[0]);
        server.setServerConfiguration(argc, argv); //создаем сервер
        BdWrapper connection;
        connection.createConnection(); //НУЖНО ПЕРЕДЕЛАТЬ В ПУЛ КОННЕКТОВ

        RestGetHello getHello;  //роутер
        RestGetBye getBye;
        DbClear dbClear;

        server.addResource(&getHello, "/hello");
        server.addResource(&getBye, "/bye");
        server.addResource(&dbClear,"/db/api/clear/");

        if (server.start()) {
            WServer::waitForShutdown();
            server.stop();
        }
    } catch (WServer::Exception& e) {
        std::cerr << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
    }
}
