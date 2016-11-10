#include <Wt/Http/Response>
#include <Wt/WResource>
#include <Wt/WServer>
#include "Router.h"
#include <zdb/zdb.h>

using namespace Wt;
ConnectionPool_T pool;

int main(int argc, char** argv)
{
    try {
        WServer server(argv[0]);
        server.setServerConfiguration(argc, argv); //создаем сервер

        URL_T url = URL_new("mysql://localhost/forum?user=root&password=1111");

        pool = ConnectionPool_new(url);
        ConnectionPool_start(pool);

        Router router; //роутер
        router.route(server);


        if (server.start()) {
            WServer::waitForShutdown();
            server.stop();
        }
    } catch (WServer::Exception& e) {
        std::cerr << e.what() << std::endl;
        std::cout<<"why2";
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
    }
    catch(...)
    {
        std::cout<<"why";
    }
}
