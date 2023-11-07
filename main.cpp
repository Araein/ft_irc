#include "ircserv.hpp"

int main ()
{
    int sock_tcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock_tcp == -1)
    {
        std::cerr << "Error creating server socket." << std::endl;
        return -1;
    }
    return 0;
}