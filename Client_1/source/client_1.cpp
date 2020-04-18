#include <llama.h>

#include "calculator.h"

#include <typeindex>
#include <cstdio>

int main()
{
    /*
    printf("Enter IP Version: ");
    int i;
    scanf_s("%d", &i);

    llama::ClientSocket socket = llama::createClientSocket();

    printf("Enter hostname of server: ");
    char buffer[128];
    scanf_s("%s", buffer, 128);

    socket->connectServer(buffer, "2020", i == 4 ? true : false);
    */

    llama::EventBus bus = llama::createEventBus();

    std::shared_ptr<Calculator> calc = std::make_shared<Calculator>(bus);
    std::shared_ptr<Console> con = std::make_shared<Console>(bus);

    calc->addToDefaultBus();
    con->addToDefaultBus();

    //Console con(bus);

    con->run();

    return 0;
}