#include "commands/commands.h"

#include <iostream>

std::pair<int, int> Command::lookUp(int pos, int argc, char* argv[], const int nodePos){
    CommandNode node = cmdTree[nodePos];
    if (argc - pos < 2){
        if (!node.terminal) return {2,pos};
        else return node.execution(argc, argv);
    }

    auto it = node.subordinates.find(argv[pos + 1]);
    if (it == node.subordinates.end()){
        // Switch case for each type of specifier
        switch (node.specExpected){
            case Specifier::Set:
                setName = argv[pos + 1]; 
                it = node.subordinates.find("$set");
                break;
             case Specifier::NewSetName:
                newSetName = argv[pos + 1]; 
                it = node.subordinates.find("$newSetName");
                break;
            case Specifier::Item:
                try { itemPos = std::stoi(argv[pos + 1]); }
                catch (std::logic_error) { return {1, pos + 1}; }
                it = node.subordinates.find("$item");
                break;
            case Specifier::None:
                if (node.terminal) return {5, pos + 1};
                else return {1, pos + 1};
            default:
                return {-1, pos + 1};
        }
        return lookUp(pos + 1, argc, argv, it->second);
    }
    else {
        if (node.specExpected != Specifier::None) return {2, pos};
        else return lookUp(pos + 1, argc, argv, it->second);
    }
}

/// @brief Add new command node
/// @param keyword 
/// @param terminal 
/// @param specExpected 
int Command::addCommandNode(std::string_view keyword, const Specifier& specExpected){
    int nodePos = nodeCount;
    CommandNode node(nodePos, std::string(keyword), std::string(keyword), false, specExpected, nullptr);
    cmdTree.push_back(node);
    nodeCount++;
    return nodePos;
}

int Command::addCommandNode(std::string_view keyword, const Specifier& specExpected, 
    std::function<std::pair<int, int>(int, char*[])> func){
    int nodePos = nodeCount;
    CommandNode node(nodePos, std::string(keyword), std::string(keyword), true, specExpected, func);
    cmdTree.push_back(node);
    nodeCount++;
    return nodePos;
}

/// @brief Add new command node
/// @param keyword 
/// @param dependingNode 
/// @param terminal 
/// @param specExpected 
int Command::addCommandNode(std::string_view keyword, const Specifier& specExpected, int dependingNode){
    int nodePos = nodeCount;
    CommandNode node(nodePos, cmdTree[dependingNode].name + "_" + std::string(keyword), std::string(keyword), false, specExpected, nullptr);
    cmdTree.push_back(node);
    cmdTree[dependingNode].subordinates[std::string(keyword)] = nodePos;
    nodeCount++;
    return nodePos;
}

/// @brief Add new command node
/// @param keyword 
/// @param dependingNode 
/// @param func 
/// @param terminal 
/// @param specExpected 
int Command::addCommandNode(std::string_view keyword, const Specifier& specExpected, int dependingNode,
    std::function<std::pair<int, int>(int, char*[])> func){
    int nodePos = nodeCount;
    CommandNode node(nodePos, cmdTree[dependingNode].name + "_" + std::string(keyword), std::string(keyword), true, specExpected, func);
    cmdTree.push_back(node);
    cmdTree[dependingNode].subordinates[std::string(keyword)] = nodePos;
    nodeCount++;
    return nodePos;
}

void Command::addSubordinate(const int sub, const int dependingNode){
    cmdTree[dependingNode].subordinates[std::string(cmdTree[sub].keyword)] = sub;
    return;
}

// EXECUTING FUNCTIONS
std::pair<int, int> Command::Zeus(int argc, char* argv[]){
    std::cout<< "ALL RETURN TO ME!";
    return {0, 0};
}

std::pair<int, int> Command::quiz_help(int argc, char* argv[]){
    std::cout << "Call an ambulance!\n";
    return {0,0};
}

std::pair<int, int> Command::quiz_about(int argc, char* argv[]){
    std::cout << "Check out the whole repo at: https://github.com/danhkhai07/cli-io-flashcard\n";
    return {0,0};
}

std::pair<int, int> Command::quiz_new_set_$set(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return {0,0};
}

std::pair<int, int> Command::quiz_new_set_$set_item_$item(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return {0,0};
}

std::pair<int, int> Command::quiz_learn_set_$set(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return {0,0};
}

std::pair<int, int> Command::quiz_learn_set_$set_item_$item(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return {0,0};

}

std::pair<int, int> Command::quiz_delete_set_$set(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return {0,0};

}

std::pair<int, int> Command::quiz_delete_set_$set_item_$item(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return {0,0};

}

std::pair<int, int> Command::quiz_rename_set_$set_$newSetName(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return {0,0};
    
}

std::pair<int, int> Command::quiz_set(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return {0,0};

}

std::pair<int, int> Command::quiz_set_$set(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return {0,0};

}

std::pair<int, int> Command::quiz_set_$set_item_$item(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return {0,0};

}
