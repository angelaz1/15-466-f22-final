#pragma once

#include <glm/glm.hpp>

#include "data_path.hpp"
#include "json.hpp"

#include <algorithm>
#include <filesystem> // https://en.cppreference.com/w/cpp/filesystem
#include <fstream>
#include <iostream>
#include <sstream>
#include <string> 
#include <vector>

struct DialogueManager {
    // constructor and destructor
    DialogueManager();
    ~DialogueManager();

    struct DialogueChoice {
        int pid;
        std::string choice_text;
    };

    struct DialogueNode {
        std::string character;
        std::string startBeatmap;
        std::string text;
        std::vector<DialogueChoice*> choices;
    };

    struct DialogueTree {
        std::unordered_map<int, DialogueNode*> dialogue_nodes;
        int start_node_pid;

        int current_node_pid;
        void start_tree() {
            current_node_pid = start_node_pid;
        };

        DialogueNode *get_current_node() {
            auto res = dialogue_nodes.find(current_node_pid);
            if (res != dialogue_nodes.end()) {
                return res->second;
            } else {
                std::cout << "ERROR: invalid current node pid" << std::endl;
                return nullptr;
            }
        };
    };

    std::unordered_map<std::string, DialogueTree*> all_dialogue;

    DialogueTree *get_dialogue_tree(std::string dialogue_name);

private:
    DialogueTree *read_dialogue(std::string file_name);
};
