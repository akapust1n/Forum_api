#include <Wt/Http/Response>
#include <Wt/WResource>
#include <Wt/WServer>
#include <BdWrapper.h>
#include "Router.h"

using namespace Wt;


int main(int argc, char** argv)
{
    try {
        WServer server(argv[0]);
        server.setServerConfiguration(argc, argv); //создаем сервер
        BdWrapper connection;
        connection.createConnection(); //НУЖНО ПЕРЕДЕЛАТЬ В ПУЛ КОННЕКТОВ
                                  //роутер
        Router router;
        router.route(server);

        //DbClear dbClear;
        //server.addResource(&dbClear,"/db/api/clear/");

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
