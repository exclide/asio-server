#include "ChatServer.h"

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cerr << "Correct usage: serv <port>\n";
        return EXIT_FAILURE;
    }

    const int numThreads = 3;
    int port = argc == 2 ? std::stoi(argv[1]) : 1234;

    io_context ioc{numThreads}; //hint number of threads running ioc
    tcp::endpoint endpoint(tcp::v4(), port);

    std::make_shared<ChatServer>(ioc, endpoint)->StartAccept();
    //keep the temp shared alive using shared from this
    std::cout << "Listening on port: " << endpoint.port() << std::endl;

    std::vector<std::thread> threads; //thread pool executing ioc queue
    for (int i = 0; i < numThreads - 1; i++) {
        threads.emplace_back([&ioc]() { ioc.run(); });
        threads[i].detach();
    }

    ioc.run();
    return EXIT_SUCCESS;
}
