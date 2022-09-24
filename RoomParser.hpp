

#ifndef INC_15_466_F22_GAME4_ROOMPARSER_HPP
#define INC_15_466_F22_GAME4_ROOMPARSER_HPP
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <map>

struct Room {
    std::string main_text;
    std::vector<std::string> choices;
    std::map<std::string, std::string> inputs;
};

struct RoomParser {
    RoomParser() = default;

    Room parse_room(std::string);
};

#endif //INC_15_466_F22_GAME4_ROOMPARSER_HPP