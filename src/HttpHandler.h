//
// Created by w on 08.12.23.
//

#ifndef SERVER_HTTPHANDLER_H
#define SERVER_HTTPHANDLER_H

#include "Asio.h"
#include "AuthService.h"
#include "jwt-cpp/jwt.h"


class HttpHandler {
public:
    explicit HttpHandler(
            const std::shared_ptr<AuthService>& authService,
            const std::string& secretJwtKey = "123");

    template<class Body, class Allocator>
    std::pair<http::message_generator, bool> HandleRequest(
            http::request<Body, http::basic_fields<Allocator>>&& req) {
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

        if (path == "/api/users") {
            if (req.method() == http::verb::get) {

            } else if (req.method() == http::verb::post) {
                auto body = req.body();
                std::cout << body << std::endl;
                json j = json::parse(body);
                User user = j.template get<User>();

                std::cout << "Registering user with login: " << user.login << std::endl;
                try {
                    if (auto dbUser = authService->Register(user); !dbUser.login.empty()) {
                        json rj = dbUser;
                        std::cout << "Pass hash: " << dbUser.password << std::endl;
                        return {ok_response(nlohmann::to_string(rj)), false};
                    } else {
                        return {unauthorized(), false};
                    }
                } catch (...) {
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

                try {
                    if (auto dbUser = authService->Login(user); !dbUser.login.empty()) {
                        json rj = dbUser;
                        auto token = jwt::create()
                                .set_issuer("wschat")
                                .set_type("JWS")
                                .set_payload_claim("login", jwt::claim(dbUser.login))
                                .sign(jwt::algorithm::hs256{secretJwtKey});

                        rj["Token"] = token;
                        std::cout << "Pass hash: " << dbUser.password << std::endl;
                        return {ok_response(nlohmann::to_string(rj)), true};
                    } else {
                        return {unauthorized(), false};
                    }
                } catch (...) {
                    return {unauthorized(), false};
                }
            } else {
                return {bad_request("Illegal request"), false};
            }
        }

        return {not_found(path), false};
    }

    std::string GetJwtKey();

private:
    std::string secretJwtKey;
    std::shared_ptr<AuthService> authService;
};


#endif //SERVER_HTTPHANDLER_H
