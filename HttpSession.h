//
// Created by w on 01.12.23.
//

#ifndef ASIO_SERVER_HTTPSESSION_H
#define ASIO_SERVER_HTTPSESSION_H

#include "Asio.h"
#include "AuthService.h"

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

    std::string path = req.target();

    auto authService = AuthService::GetInstance();

    if (path == "/api/users") {
        if (req.method() == http::verb::get) {

        } else if (req.method() == http::verb::post) {
            auto body = req.body();
            json j(body);
            User user = j.template get<User>();

            std::cout << "Registering user with login: " << user.login << std::endl;
            authService->Register(user);
        } else {
            return {bad_request("Illegal request"), false};
        }
    } else if (path == "/api/login") {
        if (req.method() == http::verb::post) {
            auto body = req.body();
            json j(body);
            User user = j.template get<User>();

            if (authService->Login(user)) {
                //login success, should now accept websocket upgrade request
                return {ok_response(json(user)), true};
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
    sslContext(ctx),
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
        auto res = HandleRequest(std::move(req));

        DoWrite(res.first);

        if (res.second) { //login success
            std::make_shared<ChatSession>(stream.next_layer().release_socket(), sslContext, room)->Start();
        }

        DoClose();

    }

    void DoWrite(http::message_generator& msg) {
        beast::async_write(
                stream,
                std::move(msg),
                [self = shared_from_this()](error_code err, size_t) {
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
    boost::asio::ssl::context& sslContext;
};

#endif //ASIO_SERVER_HTTPSESSION_H
