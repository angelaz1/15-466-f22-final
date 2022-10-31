#include "DialogueManager.hpp"

using json = nlohmann::json;

DialogueManager::DialogueManager() {
    all_dialogue = std::unordered_map<std::string, DialogueTree*>();
}

DialogueManager::~DialogueManager() {}

DialogueTree *DialogueManager::read_dialogue(std::string file_name) {
    std::string file_path = data_path("dialogue/" + file_name + ".json");

    // Reading text file into a string 
    // Referenced https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
    std::ifstream dialogue_file(file_path);
    std::stringstream buffer;
    buffer << dialogue_file.rdbuf();

    std::string data = buffer.str();
    auto json_data = json::parse(data);

    DialogueTree *tree = new DialogueTree();
    tree->dialogue_nodes = std::unordered_map<int, DialogueNode*>();

    for (auto const &passage_node : json_data["passages"]) {
        DialogueNode *node = new DialogueNode();

        // Parse text
        std::string text = passage_node["text"];
        std::cout << text << std::endl;

        if (text.length() != 0) {
            // 3+ lines within the text field
            // First line holds all the parameters for this node
            // Second line has actual text
            // 3+ lines hold the choices
            std::stringstream text_stream(text);
            std::string line;

            { // grab the first line
                std::getline(text_stream, line, '\n');

                auto text_data = json::parse(line);
                node->character = text_data["character"];

                if (text_data.contains("startBeatmap")) {
                    node->startBeatmap = true;
                    node->beatmapPath = text_data["startBeatmap"];
                } else {
                    node->startBeatmap = false;
                }
            }

            { // grab the second line
                std::getline(text_stream, line, '\n');
                node->text = line;
            }

            { // grab the remaining lines for choices
                node->choices = std::vector<DialogueChoice*>();
                if (passage_node.contains("links")) {
                    for (auto const &link : passage_node["links"]) {
                        DialogueChoice *choice = new DialogueChoice();
                        choice->choice_text = link["name"];
                        std::cout << link["name"] << std::endl;

                        std::string s_pid = link["pid"];
                        choice->pid = std::stoi(s_pid);

                        node->choices.push_back(choice);
                    }
                }
            }
        }

        int pid;
        { // Grab node pid
            std::string s_pid = passage_node["pid"];
            pid = std::stoi(s_pid);
        }

        std::cout << pid << std::endl;
        std::cout << std::endl;

        tree->dialogue_nodes.insert(std::pair(pid, node));
    }

    std::string s_pid = json_data["startnode"];
    tree->start_node_pid = std::stoi(s_pid);

    all_dialogue.insert(std::pair(file_name, tree));
    return tree;
}

DialogueTree *DialogueManager::get_dialogue_tree(std::string dialogue_name) {
    auto res = all_dialogue.find(dialogue_name);
    if (res != all_dialogue.end()) {
        return res->second;
    } else {
        return read_dialogue(dialogue_name);
    }
}