#include <llama.h>

int main()
{
    llama::ServerSocket socket = llama::createServerSocket(nullptr, 2020, false);

    return 0;
}