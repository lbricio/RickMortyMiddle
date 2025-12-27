#define BOOST_SYSTEM_NO_LIB
#define BOOST_JSON_STANDALONE

#include <boost/json/src.hpp>
#include <boost/json.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/url.hpp>

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <thread>
#include <functional>
#include <chrono>

#include "http_client.hpp"
#include "models.hpp"
#include "api.hpp"
#include "utils.hpp"

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
namespace json  = boost::json;

template<class F>
auto with_retry(F&& f, int retries = 3) {
    for (int i = 0; i < retries; ++i) {
        try { return f(); }
        catch (...) {
            if (i == retries-1) throw;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
    throw std::runtime_error("retry failed");
}

void handle_session(beast::tcp_stream& stream, RickAndMortyApi& api) {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(stream, buffer, req);

        std::string path = std::string(req.target());

        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::content_type, "application/json");

        if (path == "/help") {
            json::object h;
            h["service"] = "RickAndMorty Middleware";
            json::array cmds{"help", "character/<id>", "character/all", "exit"};
            h["commands"] = cmds;
            res.body() = json::serialize(h);
            http::write(stream, res);
            return;
        }

        if (path.starts_with("/character/all")) {
            auto list = with_retry([&]{ return api.get_all_characters_basic(); });

            json::array arr;
            for (auto const& [id, name] : list) {
                arr.push_back({{"id", id}, {"name", name}});
            }

            json::object out;
            out["characters"] = arr;

            res.body() = json::serialize(out);
            http::write(stream, res);
            return;
        }

		if (path.starts_with("/character/")) {
			std::string id_part = path.substr(strlen("/character/"));

			if (std::all_of(id_part.begin(), id_part.end(), ::isdigit)) {
				int id = std::stoi(id_part);
				auto c = with_retry([&]{ return api.get_character(id); });

				json::object o;
				o["id"]      = c.id;
				o["name"]    = c.name;
				o["status"]  = c.status;
				o["species"] = c.species;
				o["gender"]  = c.gender;
				o["origin"]  = c.origin_name;
				o["location"]= c.location_name;

				json::array eps;
				for (int eid : c.episode_ids) eps.push_back(eid);
				o["episodes"] = eps;

				res.body() = json::serialize(o);
				res.result(http::status::ok);
				http::write(stream, res);
				return;
			}
			else {
				if (id_part == "all") {
					res.body() = R"({"error":"use /character/all for full list"})";
					res.result(http::status::bad_request);
					http::write(stream, res);
					return;
				}

				for (char ch : id_part) {
					if (!::isdigit(ch) && ch != ',') {
						res.body() = R"({"error":"IDs must be numeric and comma-separated"})";
						res.result(http::status::bad_request);
						http::write(stream, res);
						return;
					}
				}

				std::vector<int> ids;
				std::stringstream ss(id_part);
				std::string tok;
				while (std::getline(ss, tok, ',')) {
					if (!tok.empty())
						ids.push_back(std::stoi(tok));
				}

				json::array arr;
				for (int cid : ids) {
					Character c = api.get_character(cid);
					arr.push_back({{"id", c.id}, {"name", c.name}});
				}

				json::object out;
				out["characters"] = arr;

				res.body() = json::serialize(out);
				res.result(http::status::ok);
				http::write(stream, res);
				return;
			}
		}

        res.body() = R"({"error":"route not found"})";
        res.result(http::status::not_found);
        http::write(stream, res);
    }
    catch (std::exception const& ex) {
        json::object err{{"error", ex.what()}};
        http::response<http::string_body> res{http::status::bad_request, 11};
        res.set(http::field::content_type, "application/json");
        res.body() = json::serialize(err);
        http::write(stream, res);
    }
}

int main() {
    HttpClient client(false);
    RickAndMortyApi api(client);

    net::io_context ioc;
    net::ip::tcp::acceptor acceptor{ioc, {net::ip::tcp::v4(), 8080}};

    std::cout << "Middleware start at port http://localhost:8080\n";

    while (true) {
        auto socket = acceptor.accept();
        beast::tcp_stream stream(std::move(socket));
        handle_session(stream, api);
    }

    return 0;
}
