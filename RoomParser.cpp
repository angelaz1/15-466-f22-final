#include "RoomParser.hpp"
#include "data_path.hpp"
#include <iostream>
#include <fstream>

Room RoomParser::parse_room(std::string path) {
    Room room;
    std::ifstream room_file(data_path("rooms/" + path).c_str());
    if(!room_file.is_open()) {
        std::cerr << path << " did not open!\n";
    }

    std::string text;
    std::getline(room_file, text, '\0');


    size_t first_choice_index = text.find_first_of('[');
    if(first_choice_index != std::string::npos) {
        room.main_text = text.substr(0, first_choice_index);
        text = text.substr(first_choice_index);
        assert(text.length() > 1);
    }

    while(text.find_first_of('[') != std::string::npos) {
        first_choice_index = text.find_first_of('[');
        text = text.substr(first_choice_index);

        //separated to check for malformed text files
        size_t first_colon = text.find_first_of(':');
        if (first_colon == std::string::npos) std::cerr << "Missing colon!\n";
        size_t first_closed_bracket = text.find_first_of(']');
        if (first_closed_bracket == std::string::npos) std::cerr << "Missing bracket!\n";
        size_t first_open_paren = text.find_first_of('(');
        if (first_open_paren == std::string::npos) std::cerr << "Missing open paren!\n";
        size_t first_closed_paren = text.find_first_of(')');
        if (first_closed_paren == std::string::npos) std::cerr << "Missing closed paren!\n";

        std::string number = text.substr(1, first_colon - 1);
        std::string choice_text = text.substr(1, first_closed_bracket - 1);
        std::string new_path = text.substr(first_open_paren + 1, first_closed_paren - first_open_paren - 1);

        room.choices.push_back(choice_text);
        room.inputs.insert(std::pair< std::string, std::string >(number, new_path));

        if(text.at(first_closed_paren + 1) == '\0') return room;
        text = text.substr(first_closed_paren + 1);
    }

    size_t first_word_index = text.find_first_of('{');
    text = text.substr(first_word_index);
    assert(text.length() > 1);

    while(text.find_first_of('{') != std::string::npos) {
        first_word_index = text.find_first_of('{');
        text = text.substr(first_word_index);

        //separated to check for malformed text files
        size_t first_closed_brace = text.find_first_of('}');
        if (first_closed_brace == std::string::npos) std::cerr << "Missing brace!\n";
        size_t first_open_paren = text.find_first_of('(');
        if (first_open_paren == std::string::npos) std::cerr << "Missing open paren!\n";
        size_t first_closed_paren = text.find_first_of(')');
        if (first_closed_paren == std::string::npos) std::cerr << "Missing closed paren!\n";

        std::string word = text.substr(1, first_closed_brace - 1);
        std::string choice_text = text.substr(1, first_closed_brace - 1);
        std::string new_path = text.substr(first_open_paren + 1, first_closed_paren - first_open_paren - 1);

        room.choices.push_back(choice_text);
        room.inputs.insert(std::pair< std::string, std::string >(word, new_path));

        if(text.at(first_closed_paren + 1) == '\0') return room;
        text = text.substr(first_closed_paren + 1);
    }
    return room;
}