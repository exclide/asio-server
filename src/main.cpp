#include "network/ChatServer.h"
#include "database/DbConfigParser.h"
#include "database/DbConnPool.h"
#include "user/AuthService.h"
#include "user/UserRepository.h"
#include "message/MessageRepository.h"


class ServerInit {
public:
    explicit ServerInit(int port = 1234, int iocThreads = 3, const std::string& dbConfigFile = "DbConfig") :
            ioc(iocThreads),
            endpoint(tcp::v4(), port),
            numThreads(iocThreads),
            signals(ioc, SIGINT, SIGTERM) {
        dbConfig = ParseDbConfig(dbConfigFile);
        dbConnPool =
                std::make_shared<DbConnPool>(dbConfig.GetDbConnectionString(), dbConfig.connections);

        signals.async_wait([this](error_code err, auto signal) {
            if (!err) {
                std::cout << "Received SIGTERM, shutting down\n";
                ioc.stop();
            } else {
                std::cout << err.message() << std::endl;
            }
        });
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

private:
    io_context ioc;
    tcp::endpoint endpoint;
    std::shared_ptr<DbConnPool> dbConnPool;
    DbConfig dbConfig;
    int numThreads;
    boost::asio::signal_set signals;
};

int main(int argc, char* argv[]) {
    if (argc > 4) {
        std::cout << "Correct usage: serv <port> <threads> <dbconfig>";
        return EXIT_FAILURE;
    }

    int port = argc > 1 ? std::stoi(argv[1]) : 1234;
    int threads = argc > 2 ? std::stoi(argv[2]) : 3;
    std::string dbConfig = argc > 3 ? argv[3] : "cfg/DbConfig";

    ServerInit(port, threads, dbConfig).Run();

    return EXIT_SUCCESS;
}
