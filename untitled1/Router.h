#ifndef ROUTER_H
#define ROUTER_H
#include "DB.h"
#include "Forum.h"
#include "Post.h"
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
        server.addResource(&forumDetails, "/db/api/forum/details/");
        server.addResource(&forumListPosts, "/db/api/forum/listPosts/");
        server.addResource(&forumListThreads, "/db/api/forum/listThreads/");
        server.addResource(&forumListUsers, "/db/api/forum/listUsers/");

        server.addResource(&userCreate, "/db/api/user/create/");
        server.addResource(&userDetails, "/db/api/user/details/");
        server.addResource(&userFollow, "/db/api/user/follow/");
        server.addResource(&userUnFollow, "/db/api/user/unfollow/");
        server.addResource(&userUpdateProfile, "/db/api/user/updateProfile/");
        server.addResource(&userListFollowers, "/db/api/user/listFollowers/");


        server.addResource(&threadCreate, "/db/api/thread/create/");
        server.addResource(&threadDetails, "/db/api/thread/details/");
        server.addResource(&threadClose, "/db/api/thread/close/");
        server.addResource(&threadList, "/db/api/thread/list/");
        server.addResource(&threadListPost, "/db/api/thread/listPosts/");
        server.addResource(&threadRemove, "/db/api/thread/remove/");
        server.addResource(&threadRestore, "/db/api/thread/restore/");
        server.addResource(&threadOpen, "/db/api/thread/open/");
        server.addResource(&threadUpdate, "/db/api/thread/update/");
        server.addResource(&threadVote, "/db/api/thread/vote/");
        server.addResource(&threadSubscribe, "/db/api/thread/subscribe/");
        server.addResource(&threadUnSubscribe, "/db/api/thread/unsubscribe/");

        server.addResource(&postCreate, "/db/api/post/create/");
        server.addResource(&postDetails, "/db/api/post/details/");
        server.addResource(&postRemove, "/db/api/post/remove/");
        server.addResource(&postRestore, "/db/api/post/restore/");
        server.addResource(&postUpdate, "/db/api/post/update/");
        server.addResource(&postVote, "/db/api/post/vote/");
        server.addResource(&postList, "/db/api/post/list/");

    }

private: // :)
    DbClear dbClear;
    DbStatus dbStatus;

    ForumCreate forumCreate;
    ForumDetails forumDetails;
    ForumListPosts forumListPosts;
    ForumListThreads forumListThreads;
    ForumListUsers forumListUsers;

    UserCreate userCreate;
    UserDetails userDetails;
    UserFollow userFollow;
    UserUpdateProfile userUpdateProfile;
    UserListFollowers userListFollowers;
    UserUnFollow userUnFollow;

    ThreadCreate threadCreate;
    ThreadDetails threadDetails;
    ThreadClose threadClose;
    ThreadRemove threadRemove;
    ThreadRestore threadRestore;
    ThreadList threadList;
    ThreadListPost threadListPost;
    ThreadOpen threadOpen;
    ThreadUpdate threadUpdate;
    ThreadVote threadVote;
    ThreadSubscribe threadSubscribe;
    ThreadUnSubscribe threadUnSubscribe;

    PostCreate postCreate;
    PostDetails postDetails;
    PostRemove postRemove;
    PostRestore postRestore;
    PostUpdate postUpdate;
    PostVote postVote;
    PostList postList;



};

#endif // ROUTER_H
