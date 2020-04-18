#include <llama.h>

int main()
{
    llama::ServerSocket socket = llama::createServerSocket(2020, false);

    return 0;
}