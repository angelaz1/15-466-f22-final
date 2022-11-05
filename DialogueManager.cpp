#include "DialogueManager.hpp"

using json = nlohmann::json;

DialogueManager::DialogueManager() {
    all_dialogue = std::unordered_map<std::string, DialogueTree*>();
}

DialogueManager::~DialogueManager() {}

void process_parameters(DialogueNode *node, nlohmann::json_abi_v3_11_2::json text_data) {
    if (text_data.contains("character")) {
        node->character = text_data["character"];
    } else {
        node->character = "";
    }

    if (text_data.contains("startBeatmap")) {
        node->startBeatmap = true;
        node->beatmapPath = text_data["startBeatmap"];
    } else {
        node->startBeatmap = false;
    }

    if (text_data.contains("relationship")) {
        node->relationshipChange = text_data["relationship"];
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
            relationship_points += current_node->relationshipChange;
        }
    }
    else {
        std::cout << "Invalid choice index provided: " << index << std::endl;
    }
}