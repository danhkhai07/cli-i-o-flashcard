#include "commands/commands.h"

#include <iostream>

ExecutingOutput Command::lookUp(int pos, int argc, char* argv[], const int nodePos){
    CommandNode node = cmdTree[nodePos];
    if (argc - pos < 2){
        if (!node.terminal) return ExecutingOutput(2, pos, "");
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
                catch (std::logic_error) { return ExecutingOutput(1, pos + 1); }
                it = node.subordinates.find("$item");
                break;
            case Specifier::None:
                if (node.terminal) return ExecutingOutput(5, pos + 1);
                else return ExecutingOutput(1, pos + 1);
            case Specifier::Other:
                if (node.terminal) {
                    return node.execution(argc, argv);
                }
                else return ExecutingOutput(-1, pos);

            default:
                return ExecutingOutput(-1, pos + 1);
        }
        return lookUp(pos + 1, argc, argv, it->second);
    }
    else {
        if (node.specExpected != Specifier::None) return ExecutingOutput(2, pos);
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
    std::function<ExecutingOutput(int, char*[])> func){
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
    std::function<ExecutingOutput(int, char*[])> func){
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
ExecutingOutput Command::Zeus(int argc, char* argv[]){
    std::cout<< "ALL RETURN TO ME!";
    return ExecutingOutput(0, 0);
}

ExecutingOutput Command::quiz_help(int argc, char* argv[]){
    std::cout << "Call an ambulance!\n";
    return ExecutingOutput(0, 0);
}

ExecutingOutput Command::quiz_about(int argc, char* argv[]){
    std::cout << "Check out the whole repo at: https://github.com/danhkhai07/cli-io-flashcard\n";
    return ExecutingOutput(0, 0);
}

ExecutingOutput Command::quiz_new_set_$set(int argc, char* argv[]){
    // std::cout << "Reached.\n";
    ExecutingOutput exeOut;
    if (argc <= 4){
        std::string option_s = "-s, --set <SETNAME>\tName of the new set";
        exeOut.options.push_back(option_s);
        int exeCode = DataHandler.newSet(setName);
        exeOut.errorCode = exeCode;
        if (exeCode != 0) exeOut.errorPos = 3;
        return exeOut;
    }

    exeOut.otherspecArgumentGuide = "--front <CONTENT> --back <CONTENT>";
    std::string option_s = "-s, --set <SETNAME>\tName of the existing set";
    std::string option_f = "-f, --front <CONTENT\tSet the front content (required)";
    std::string option_b = "-b, --back <CONTENT>\tSet the back content (required)";
    exeOut.options.push_back(option_s);
    exeOut.options.push_back(option_f);
    exeOut.options.push_back(option_b);

    std::string front = "", back = "";
    bool f_arg = false, b_arg = false;
    for (int i = 4; i < argc; i++){
        if (argv[i] == "--front" or argv[i] == "-f"){
            if (argv[i + 1] == nullptr){
                exeOut.errorCode = 2;
                exeOut.errorPos = i;
                return exeOut;
            }
            front = argv[i++];
            f_arg = true;
        } else if (argv[i] == "--back" or argv[i] == "-b"){
            if (argv[i + 1] == nullptr){
                exeOut.errorCode = 2;
                exeOut.errorPos = i;
                return exeOut;
            }
            back = argv[i++];
            b_arg = true;
        } else {
            exeOut.errorCode = 5;
            exeOut.errorPos = i;
            return exeOut;
        }
    }
    
    int res = DataHandler.addCard(setName, front, back);
    exeOut.errorCode = res;
    if (res != 0) exeOut.errorPos = 4;
    return exeOut;
}

ExecutingOutput Command::quiz_learn_set_$set(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return ExecutingOutput(0, 0, "");
}

ExecutingOutput Command::quiz_learn_set_$set_item_$item(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return ExecutingOutput(0, 0, "");
}

ExecutingOutput Command::quiz_delete_set_$set(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return ExecutingOutput(0, 0, "");
}

ExecutingOutput Command::quiz_delete_set_$set_item_$item(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return ExecutingOutput(0, 0, "");
}

ExecutingOutput Command::quiz_rename_set_$set_$newSetName(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return ExecutingOutput(0, 0, "");
}

ExecutingOutput Command::quiz_set(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return ExecutingOutput(0, 0, "");
}

ExecutingOutput Command::quiz_set_$set(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return ExecutingOutput(0, 0, "");
}

ExecutingOutput Command::quiz_set_$set_item_$item(int argc, char* argv[]){
    std::cout << "Reached.\n";
    return ExecutingOutput(0, 0, "");
}
