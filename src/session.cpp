#include "session.hpp"
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>
#include <sstream>
#include "api.hpp"
#include "utils.hpp"
namespace beast = boost::beast;
namespace http  = beast::http;
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

Session::Session(beast::tcp_stream& stream, RickAndMortyApi& api)
    : stream_(stream), api_(api) {}

void Session::handle() {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(stream_, buffer, req);

        std::string path = std::string(req.target());

        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::content_type, "application/json");

        if (path == "/help") {
            json::object h;
            h["service"] = "RickAndMorty Middleware";
            json::array cmds{"help", "character/<id>", "character/all", "exit"};
            h["commands"] = cmds;
            res.body() = json::serialize(h);
            http::write(stream_, res);
            return;
        }

        if (path.starts_with("/character/all")) {
            auto list = with_retry([&]{ return api_.get_all_characters_basic(); });
            json::array arr;
            for (auto const& [id, name] : list) {
                arr.push_back({{"id", id}, {"name", name}});
            }
            json::object out;
            out["characters"] = arr;
            res.body() = json::serialize(out);
            http::write(stream_, res);
            return;
        }

        if (path.starts_with("/character/")) {
            std::string id_part = path.substr(strlen("/character/"));

            if (std::all_of(id_part.begin(), id_part.end(), ::isdigit)) {
                int id = std::stoi(id_part);
                auto c = with_retry([&]{ return api_.get_character(id); });
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
                http::write(stream_, res);
                return;
            }

            if (id_part == "all") {
                res.body() = R"({"error":"use /character/all for full list"})";
                res.result(http::status::bad_request);
                http::write(stream_, res);
                return;
            }

            for (char ch : id_part) {
                if (!::isdigit(ch) && ch != ',') {
                    res.body() = R"({"error":"IDs must be numeric and comma-separated"})";
                    res.result(http::status::bad_request);
                    http::write(stream_, res);
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
                Character c = api_.get_character(cid);
                arr.push_back({{"id", c.id}, {"name", c.name}});
            }

            json::object out;
            out["characters"] = arr;
            res.body() = json::serialize(out);
            res.result(http::status::ok);
            http::write(stream_, res);
            return;
        }

        res.body() = R"({"error":"route not found"})";
        res.result(http::status::not_found);
        http::write(stream_, res);
    }
    catch(std::exception const& e) {
        json::object err{{"error", e.what()}};
        http::response<http::string_body> res{http::status::bad_request, 11};
        res.set(http::field::content_type, "application/json");
        res.body() = json::serialize(err);
        http::write(stream_, res);
    }
}
