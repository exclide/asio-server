//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_HTTPSESSION_H
#define ASIO_SERVER_HTTPSESSION_H

#include "Asio.h"
#include "AuthService.h"
#include "User.h"
#include "jwt-cpp/jwt.h"

std::string SECRET_JWT_KEY = "123";

// The concrete type of the response message (which depends on the
// request), is type-erased in message_generator.
template<class Body, class Allocator>
std::pair<http::message_generator, bool> HandleRequest(http::request<Body, http::basic_fields<Allocator>>&& req) {
    auto const bad_request =
            [&req](beast::string_view why)
            {
                http::response<http::string_body> res{http::status::bad_request, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = std::string(why);
                res.prepare_payload();
                return res;
            };

    auto const not_found =
            [&req](beast::string_view target)
            {
                http::response<http::string_body> res{http::status::not_found, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = "The resource '" + std::string(target) + "' was not found.";
                res.prepare_payload();
                return res;
            };

    auto const unauthorized =
            [&req]()
            {
                http::response<http::string_body> res{http::status::unauthorized, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = "Bad credentials";
                res.prepare_payload();
                return res;
            };

    auto const ok_response =
            [&req](beast::string_view json) {
                http::response<http::string_body> res{http::status::ok, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = json;
                res.prepare_payload();
                return res;
            };

    // Make sure we can handle the method
    if(req.method() != http::verb::get &&
       req.method() != http::verb::post)
        return {bad_request("Unknown HTTP-method"), false};

    // Request path must be absolute and not contain "..".
    if(req.target().empty() ||
       req.target()[0] != '/' ||
       req.target().find("..") != beast::string_view::npos)
        return {bad_request("Illegal request-target"), false};

    std::cout << req.target() << std::endl;
    std::cout << req.method() << std::endl;

    std::string path = req.target();

    auto authService = AuthService::GetInstance();

    if (path == "/api/users") {
        if (req.method() == http::verb::get) {

        } else if (req.method() == http::verb::post) {
            auto body = req.body();
            std::cout << body << std::endl;
            json j = json::parse(body);
            User user = j.template get<User>();

            std::cout << "Registering user with login: " << user.login << std::endl;
            if (auto dbUser = authService->Register(user); !dbUser.login.empty()) {
                json rj = dbUser;
                std::cout << "Pass hash: " << dbUser.password << std::endl;
                return {ok_response(nlohmann::to_string(rj)), false};
            } else {
                return {unauthorized(), false};
            }

        } else {
            return {bad_request("Illegal request"), false};
        }
    } else if (path == "/api/login") {
        if (req.method() == http::verb::post) {
            auto body = req.body();
            std::cout << body << std::endl;
            json j = json::parse(body);
            User user = j.template get<User>();

            if (auto dbUser = authService->Login(user); !dbUser.login.empty()) {
                json rj = dbUser;
                auto token = jwt::create()
                        .set_issuer("wschat")
                        .set_type("JWS")
                        .set_payload_claim("login", jwt::claim(dbUser.login))
                        .sign(jwt::algorithm::hs256{SECRET_JWT_KEY});

                rj["Token"] = token;
                std::cout << "Pass hash: " << dbUser.password << std::endl;
                return {ok_response(nlohmann::to_string(rj)), true};
            } else {
                return {unauthorized(), false};
            }
        } else {
            return {bad_request("Illegal request"), false};
        }
    }

    return {not_found(path), false};
}

class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
    HttpSession(tcp::socket&& socket, boost::asio::ssl::context& ctx, std::shared_ptr<ChatRoom>& room) :
    stream(std::move(socket), ctx),
    room(room) {
        std::cout << "Http connection open\n";
    }

    ~HttpSession() {
        std::cout << "Http connection closed\n";
    }

    void Start() {
        boost::asio::dispatch( //the first call won't be on strand, so dispatch to strand
                stream.get_executor(),
                [self = shared_from_this()]() {
                    self->DoSslHandshake();
                });
    }

    void DoSslHandshake() {
        stream.async_handshake(
                boost::asio::ssl::stream_base::server,
                [self = shared_from_this()](error_code err){
                    if (!err) {
                        std::cout << "Http: Accepted SSL handshake from client\n";
                        self->DoRead();
                    } else {
                        std::cout << "Http: SSL handshake failed\n";
                    }
                });
    }

    void DoRead() {
        req = {};

        http::async_read(
                stream,
                buffer,
                req,
                [self = shared_from_this()](error_code err, size_t) {
                    if (err) {
                        self->DoClose();
                        return;
                    }

                    self->HandleHttpRequest();
                });
    }

    void HandleHttpRequest() {
        std::cout << "Handle request called\n";

        if (websocket::is_upgrade(req)) {
            if (!req.base().count("Authorization")) return;
            std::string token = req.base().at("Authorization");
            std::cout << token << std::endl;

            static auto verifier = jwt::verify()
                    .allow_algorithm(jwt::algorithm::hs256{SECRET_JWT_KEY})
                    .with_issuer("wschat");

            try {
                auto decoded = jwt::decode(token);
                verifier.verify(decoded);
            } catch (...) {
                std::cout << "Wrong token received\n";
                return;
            }

            std::cout << "Is upgrade request\n";
            std::make_shared<ChatSession>(
                    std::move(stream), room)->Start(req);
            return;
        }


        auto res = HandleRequest(std::move(req));

        beast::async_write(
                stream,
                std::move(res.first),
                [self = shared_from_this(), login = res.second](error_code err, size_t) {
                    if (login) {
                        std::cout << "LoginSuccess\n";
                    }
                }
        );

    }

    void DoClose() {
        stream.async_shutdown([self=shared_from_this()](error_code err) {
            if (err) {
                std::cout << err << std::endl;
            }
        });
    }


private:
    beast::ssl_stream<beast::tcp_stream> stream;
    beast::flat_buffer buffer;
    http::request<http::string_body> req;

    std::shared_ptr<ChatRoom> room;
};

#endif //ASIO_SERVER_HTTPSESSION_H
