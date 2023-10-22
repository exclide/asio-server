#include "ChatServer.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Correct usage: serv <port>\n";
        return EXIT_FAILURE;
    }

    const int numThreads = 3;

    io_context ioc{numThreads}; //hint number of threads running ioc
    tcp::endpoint endpoint(tcp::v4(), std::stoi(argv[1]));

    ChatServer server{ioc, endpoint};
    std::cout << "Listening on port: " << endpoint.port() << std::endl;

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads - 1; i++) {
        threads.emplace_back([&ioc]() { ioc.run(); });
        threads[i].detach();
    }

    ioc.run();

    return EXIT_SUCCESS;
}
