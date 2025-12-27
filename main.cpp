#define BOOST_SYSTEM_NO_LIB
#define BOOST_JSON_STANDALONE

#include <boost/json/src.hpp>
#include <boost/json.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/url.hpp>

#include <iostream>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include "handler.hpp"
#include "api.hpp"
#include "http_client.hpp"

namespace net   = boost::asio;
namespace beast = boost::beast;

int main() {
    HttpClient client(false);
    RickAndMortyApi api(client);

    net::io_context ioc;
    net::ip::tcp::acceptor acceptor{ioc, {net::ip::tcp::v4(), 8080}};

    std::cout << "Middleware started at port 8080\n";

    while (true) {
        auto socket = acceptor.accept();
        beast::tcp_stream stream(std::move(socket));

        Handler middleware(stream, api);
        middleware.handle();
    }

    return 0;
}
