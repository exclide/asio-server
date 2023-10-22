#include "ChatServer.h"
#include "ChatSession.h"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Correct usage: serv <port>\n";
        return EXIT_FAILURE;
    }

    io_context ioc;
    tcp::endpoint endpoint(tcp::v4(), std::stoi(argv[1]));

    ChatServer server{ioc, endpoint};
    std::cout << "Listening on port: " << endpoint.port() << std::endl;

    ioc.run();

    return EXIT_SUCCESS;
}
