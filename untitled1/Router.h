#ifndef ROUTER_H
#define ROUTER_H
#include "DB.h"
#include "Forum.h"
#include "User.h"
#include <Thread.h>
#include <Wt/WResource>
#include <Wt/WServer>

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
        server.addResource(&userFollow, "/db/api/user/follow/");
        server.addResource(&userUpdateProfile, "/db/api/user/updateProfile/");
        server.addResource(&userListFollowers, "/db/api/user/listFollowers/");
        server.addResource(&threadCreate, "/db/api/thread/create/");
        server.addResource(&threadDetails, "/db/api/thread/details/");
        server.addResource(&threadDetails, "/db/api/thread/close/");
        server.addResource(&threadList, "/db/api/thread/list/");
        server.addResource(&threadRemove, "/db/api/thread/remove/");
    }

private: // :)
    DbClear dbClear;
    DbStatus dbStatus;
    UserCreate userCreate;
    UserDetails userDetails;
    ForumCreate forumCreate;
    UserFollow userFollow;
    UserUpdateProfile userUpdateProfile;
    UserListFollowers userListFollowers;
    ThreadCreate threadCreate;
    ThreadDetails threadDetails;
    ThreadClose threadClose;
    ThreadList threadList;
    ThreadRemove threadRemove;
};

#endif // ROUTER_H
