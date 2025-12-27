#pragma once
#include <string>
#include <vector>

struct Character {
    int id{};
    std::string name;
    std::string status;
    std::string species;
    std::string gender;
    std::string origin_name;
    std::string location_name;
    std::vector<int> episode_ids;
};

struct Episode {
    std::string name;
    std::string episode;
    std::string air_date;
    std::vector<std::string> characters;
};
