#ifndef ROUTER_H
#define ROUTER_H
#include "DB.h"
#include <Wt/WResource>
#include <Wt/WServer>
#include "Forum.h"
#include "User.h"

using namespace Wt;

const int API_SIZE = 32;

class Router {
public:
    void route(WServer& server)
    {
        server.addResource(&dbClear, "/db/api/clear/");
        server.addResource(&dbStatus, "/db/api/status/");
        server.addResource(&forumCreate, "/db/api/forum/create/");
        server.addResource(&userCreate, "/db/api/user/create/");
        server.addResource(&userDetails, "/db/api/user/details/");
    }

private: // :)
    DbClear dbClear;
    DbStatus dbStatus;
    UserCreate userCreate;
    UserDetails userDetails;
    ForumCreate forumCreate;

};

#endif // ROUTER_H
