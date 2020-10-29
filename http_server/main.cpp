#include <iostream>
#include <signal.h>
#include "httpserver.h"
using namespace std;

httpServer* server=nullptr;
void sigClose(int dummy) {
    server->closeServer();
    delete server;
    exit(0);
}

int main()
{
    server =new httpServer(8080);

    signal(SIGINT, sigClose);

    while(1)
    {
        int newfd=server->waitForConnections();
        server->resolveRequests(newfd);
    }
}
