#pragma once
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <string>
#include "api.hpp"

class Session {
public:
    Session(boost::beast::tcp_stream& stream, RickAndMortyApi& api);
    void handle();
private:
    boost::beast::tcp_stream& stream_;
    RickAndMortyApi& api_;
};
