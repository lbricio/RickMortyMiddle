#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>

#include "handler.hpp"
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
            if (i == retries - 1) throw;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
    throw std::runtime_error("retry failed");
}

void Handler::route_request(const std::string& path, const http::request<http::string_body>& req) {
    if (path == "/help") {
        help(req);
        return;
    }

    if (path.starts_with("/character/all")) {
        character_all(req);
        return;
    }

	if (path.starts_with("/character/?")) {
		std::string forward_target = "/api/character/?" + path.substr(12);
		auto body = with_retry([&]{ return api_.route_query(forward_target); });
		send_response(http::status::ok, body, req);
		return;
	}

    if (path.starts_with("/character/")) {
        std::string id_part = path.substr(strlen("/character/"));

        if (std::all_of(id_part.begin(), id_part.end(), ::isdigit)) {
            character_single(std::stoi(id_part), req);
            return;
        }

        if (id_part.find(',') != std::string::npos) {
            character_batch(id_part, req);
            return;
        }
    }

	if (path.starts_with("/location/all")) {
		location_all(req);
		return;
	}

	if (path.starts_with("/location/?")) {
		std::string forward_target = "/api/location/?" + path.substr(10);
		auto body = with_retry([&]{ return api_.route_query(forward_target); });
		send_response(http::status::ok, body, req);
		return;
	}

	if (path.starts_with("/location/")) {
		std::string id_part = path.substr(strlen("/location/"));

		if (std::all_of(id_part.begin(), id_part.end(), ::isdigit)) {
			location_single(std::stoi(id_part), req);
			return;
		}

		if (id_part.find(',') != std::string::npos) {
			location_batch(id_part, req);
			return;
		}
	}

	if (path.starts_with("/episode/all")) {
		episode_all(req);
		return;
	}

	if (path.starts_with("/episode/?")) {
		episode_query(req);
		return;
	}

	if (path.starts_with("/episode/")) {
		std::string id_part = path.substr(9);

		if (std::all_of(id_part.begin(), id_part.end(), ::isdigit)) {
			episode_single(std::stoi(id_part), req);
			return;
		}

		if (id_part.find(',') != std::string::npos) {
			episode_batch(id_part, req);
			return;
		}
	}
	
    send_response(http::status::not_found, R"({"error":"route not found"})", req);
}
