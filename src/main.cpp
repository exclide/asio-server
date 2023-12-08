#include "ChatServer.h"
#include "DbConfigParser.h"
#include "DbConnPool.h"
#include "AuthService.h"
#include "UserRepository.h"
#include "MessageRepository.h"

int main(int argc, char* argv[]) {
    if (argc > 3) {
        std::cerr << "Correct usage: serv <port> <dbconfig>\n";
        return EXIT_FAILURE;
    }

    const int numThreads = 3;

    int port = argc == 2 ? std::stoi(argv[1]) : 1234;
    std::string dbConfigFile = argc == 3 ? argv[2] : "DbConfig";

    auto dbConfig = ParseDbConfig(dbConfigFile);

    auto dbConnPool =
            std::make_shared<DbConnPool>(dbConfig.GetDbConnectionString(), dbConfig.connections);
    auto userRepository = std::make_shared<UserRepository>(dbConnPool);
    auto authService = std::make_shared<AuthService>(userRepository);
    auto msgRepository = std::make_shared<MessageRepository>(dbConnPool);
    auto msgService = std::make_shared<MessageService>(msgRepository);

    io_context ioc{numThreads}; //hint number of threads running ioc
    tcp::endpoint endpoint(tcp::v4(), port);

    std::make_shared<ChatServer>(ioc, endpoint, authService, msgService)->StartAccept();
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
