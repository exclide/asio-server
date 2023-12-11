//
// Created by asd on 22-Oct-23.
//

#ifndef ASIO_SERVER_ASIO_H
#define ASIO_SERVER_ASIO_H

#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>

#include <nlohmann/json.hpp>

using boost::asio::io_context;
using boost::asio::ip::tcp;
using boost::system::error_code;
using boost::asio::strand;

using json = nlohmann::json;

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;

#endif //ASIO_SERVER_ASIO_H
