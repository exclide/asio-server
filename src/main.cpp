#include "ChatServer.h"
#include "DbConfigParser.h"
#include "DbConnPool.h"
#include "AuthService.h"
#include "UserRepository.h"
#include "MessageRepository.h"


class ServerInit {
public:
    explicit ServerInit(int port = 1234, int iocThreads = 3, const std::string& dbConfigFile = "DbConfig") :
            ioc(iocThreads),
            endpoint(tcp::v4(), port),
            numThreads(iocThreads) {
        dbConfig = ParseDbConfig(dbConfigFile);
        dbConnPool =
                std::make_shared<DbConnPool>(dbConfig.GetDbConnectionString(), dbConfig.connections);
    }

    void Run() {
        auto userRepository = std::make_shared<UserRepository>(dbConnPool);
        auto authService = std::make_shared<AuthService>(userRepository);
        auto msgRepository = std::make_shared<MessageRepository>(dbConnPool);
        auto msgService = std::make_shared<MessageService>(msgRepository);

        auto sharedState = std::make_shared<SharedState>(msgService, authService);
        std::make_shared<ChatServer>(ioc, endpoint, sharedState)->StartAccept();
        //keep the temp shared alive using shared from this
        std::cout << "Listening on port: " << endpoint.port() << std::endl;

        std::vector<std::thread> threads; //thread pool executing ioc queue
        for (int i = 0; i < numThreads - 1; i++) {
            threads.emplace_back([this]() { ioc.run(); });
            threads[i].detach();
        }

        ioc.run();
    }


    io_context ioc;
    tcp::endpoint endpoint;
    std::shared_ptr<DbConnPool> dbConnPool;
    DbConfig dbConfig;
    int numThreads;
};

std::shared_ptr<ServerInit> server;

void SigtermHandler(int signal) {
    std::cout << "Received SIGTERM\n"
                 "Shutting down...\n";

    //stop the ioc loop to let the RAII destructors free resources, db connections, etc
    server->ioc.stop();
}

int main(int argc, char* argv[]) {
    if (argc > 4) {
        std::cout << "Correct usage: serv <port> <threads> <dbconfig>";
        return EXIT_FAILURE;
    }

    std::signal(SIGTERM, SigtermHandler);

    int port = argc > 1 ? std::stoi(argv[1]) : 1234;
    int threads = argc > 2 ? std::stoi(argv[2]) : 3;
    std::string dbConfig = argc > 3 ? argv[3] : "DbConfig";

    server = std::make_shared<ServerInit>(port, threads, dbConfig);
    server->Run();

    return EXIT_SUCCESS;
}
