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

struct DialogueChoice {
    int pid;
    std::string choice_text;
};

struct DialogueNode {
    enum Emotion { BLUSH, NEUTRAL, SAD, SMILE };
    bool isCheckNode;
    int minRelationship;

    std::string character;
    Emotion emotion = BLUSH;
    bool startBeatmap;
    std::string beatmapPath;
    int relationshipChange;

    std::string text;
    std::vector<DialogueChoice*> choices;
};

struct DialogueTree {
    std::unordered_map<int, DialogueNode*> dialogue_nodes;
    int start_node_pid;
    int relationship_points;

    DialogueNode* current_node;

    void start_tree();
    void choose_choice(size_t index);

private:
    DialogueNode* find_node_from_pid(int pid);
};

struct DialogueManager {
    // constructor and destructor
    DialogueManager();
    ~DialogueManager();

    std::unordered_map<std::string, DialogueTree*> all_dialogue;

    DialogueTree *get_dialogue_tree(std::string dialogue_name);

private:
    DialogueTree *read_dialogue(std::string file_name);
};
