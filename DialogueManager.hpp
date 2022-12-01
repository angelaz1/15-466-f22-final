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
    std::string new_dialogue_file;
    std::string choice_text;
};

struct DialogueNode {
    bool isCheckNode;
    int minRelationship;

    enum Emotion { BLUSH, NEUTRAL, SAD, SMILE, ANGRY };
    enum Background { CONCERT_HALL, COFFEE_SHOP, CLASSROOM, HALLWAY, OUTSIDE, BUS, BALLROOM, BEACH, FOREST, HOUSE, LIBRARY, NONE, KEEP };
    enum TextSpeed { NORMAL, SLOW, VERY_SLOW };
    enum Animation { SHAKE, BOUNCE, NO_ANIMATION };

    std::string character;      // Used as character name to display
    std::string portraitName;   // Used for portrait to display
    Emotion emotion = NEUTRAL;
    Background background = KEEP;
    TextSpeed textSpeed = NORMAL;
    Animation animation = NO_ANIMATION;

    bool startBeatmap;
    std::string beatmapPath;
    int songScoring;

    std::string dialogueColor;
    glm::uvec3 backgroundTint;

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
    DialogueTree *choose_choice(size_t index);

    std::vector<DialogueNode*> jump_nodes;
    void jump_to_next_beatmap();

private:
    DialogueNode* find_node_from_pid(int pid);
};

struct DialogueManager {

protected:
    DialogueManager();
    static DialogueManager* dialoguemanager_;

public:
    DialogueManager(DialogueManager &other) = delete;
    void operator=(const DialogueManager &) = delete;
    static DialogueManager *GetInstance();

    DialogueTree *get_dialogue_tree(std::string dialogue_name);

private:
    std::unordered_map<std::string, DialogueTree*> all_dialogue;
    DialogueTree *read_dialogue(std::string file_name);
};
