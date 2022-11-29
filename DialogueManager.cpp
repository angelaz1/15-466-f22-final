#include "DialogueManager.hpp"

using json = nlohmann::json;

DialogueManager::DialogueManager() {
    all_dialogue = std::unordered_map<std::string, DialogueTree*>();
}

DialogueManager::~DialogueManager() {}

void process_parameters(DialogueNode *node, nlohmann::json_abi_v3_11_2::json text_data) {
    if (text_data.contains("character")) {
        node->character = text_data["character"];
        node->portraitName = text_data["character"]; // Used if character name matches portrait
    } else {
        node->character = "";
        node->portraitName = "";
    }

    // Can override portrait name
    if (text_data.contains("portraitName")) {
        node->portraitName = text_data["portraitName"];
    }

    if (text_data.contains("startBeatmap")) {
        node->startBeatmap = true;
        node->beatmapPath = text_data["startBeatmap"];
    } else {
        node->startBeatmap = false;
    }

    if (text_data.contains("songScoring")) {
        node->songScoring = text_data["songScoring"];
    } else {
        node->songScoring = 0;
    }

    if (text_data.contains("relationshipChange")) {
        node->relationshipChange = text_data["relationshipChange"];
    } else {
        node->relationshipChange = 0;
    }

    if (text_data.contains("checkNode")) {
        node->isCheckNode = true;
        assert(text_data.contains("minRelationship"));
        node->minRelationship = text_data["minRelationship"];
    } else {
        node->isCheckNode = false;
    }

    if (text_data.contains("emotion")) {
        std::string emotion = text_data["emotion"];
        if (emotion.compare("angry") == 0) {
            node->emotion = DialogueNode::ANGRY;
        } else if (emotion.compare("sad") == 0) {
            node->emotion = DialogueNode::SAD;
        } else if (emotion.compare("blush") == 0) {
            node->emotion = DialogueNode::BLUSH;
        } else if (emotion.compare("smile") == 0) {
            node->emotion = DialogueNode::SMILE;
        }
    }

    if (text_data.contains("background")) {
        std::string background = text_data["background"];
        if (background.compare("concert_hall") == 0) {
            node->background = DialogueNode::CONCERT_HALL;
        } else if (background.compare("coffee_shop") == 0) {
            node->background = DialogueNode::COFFEE_SHOP;
        } else if (background.compare("classroom") == 0) {
            node->background = DialogueNode::CLASSROOM;
        } else if (background.compare("hallway") == 0) {
            node->background = DialogueNode::HALLWAY;
        } else if (background.compare("outside") == 0) {
            node->background = DialogueNode::OUTSIDE;
        } else if (background.compare("none") == 0) {
            node->background = DialogueNode::NONE;
        }
    }
}

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
    tree->jump_nodes = std::vector<DialogueNode*>();

    for (auto const &passage_node : json_data["passages"]) {
        DialogueNode *node = new DialogueNode();

        // Parse text
        std::string text = passage_node["text"];

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
                process_parameters(node, text_data);
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

        tree->dialogue_nodes.insert(std::pair(pid, node));
        if (node->startBeatmap) tree->jump_nodes.push_back(node);
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

DialogueNode *DialogueTree::find_node_from_pid(int pid) {
    auto res = dialogue_nodes.find(pid);
    if (res != dialogue_nodes.end()) {
        return res->second;
    }
    else {
        std::cout << "Invalid pid provided: " << pid << std::endl;
        return NULL;
    }
}

void DialogueTree::start_tree() {
    relationship_points = 0;
    current_node = find_node_from_pid(start_node_pid);
}

void DialogueTree::choose_choice(size_t index) {
    if (index < current_node->choices.size()) {
        int choice_node_pid = current_node->choices[index]->pid;

        DialogueNode *choice_node = find_node_from_pid(choice_node_pid);
        current_node = choice_node;
        if (current_node == jump_nodes.front()) jump_nodes.erase(jump_nodes.begin());
        relationship_points += current_node->relationshipChange;

        while (current_node->isCheckNode) {
            assert(current_node->choices.size() == 2);
            if (relationship_points >= current_node->minRelationship) {
                choice_node_pid = current_node->choices[0]->pid;
            } else {
                choice_node_pid = current_node->choices[1]->pid;
            }

            choice_node = find_node_from_pid(choice_node_pid);
            current_node = choice_node;
            if (current_node == jump_nodes.front()) jump_nodes.erase(jump_nodes.begin());
            relationship_points += current_node->relationshipChange;
        }
    }
    else {
        std::cout << "Invalid choice index provided: " << index << std::endl;
    }
}

void DialogueTree::jump_to_next_beatmap() {
    if (jump_nodes.size() == 0) return;
    current_node = jump_nodes.front();
    jump_nodes.erase(jump_nodes.begin());
}
