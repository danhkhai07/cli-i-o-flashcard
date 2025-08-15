#include "commands/commands.h"

#include <iostream>
#include <set>

static const std::map<int, std::string> ErrorCodeMessage = {
    {-1, "Internal error"},
    {0, ""},
    {1, "Invalid argument format"},
    {2, "Missing argument"},
    {3, "Unknown command"},
    {4, "Conflicting options"},
    {5, "Too many arguments"},
    {6, "Invalid name"},
    {7, "Name already exists"},
    {8, "Set does not exist"},
    {9, "Set has reached its maximum capacity (2*10^9)"},
    {10, "Card index is out of bounds"}
};

static const std::map<std::string, std::string> HelpEquivalentKeyword = {
    {"$set", "SET-NAME"},
    {"$item", "INDEX"},
    {"$newSetName", "NEW-SET-NAME"}
};

static const std::set<int> DiscontinuedMsgCode = {
    -1, 0, 2 
}; 

ExecutingOutput Command::lookUp(int pos, int argc, char* argv[], const int nodePos,
    std::vector<std::pair<std::string, std::string>> options){
    CommandNode node = cmdTree[nodePos];
    ExecutingOutput exeOut;
    exeOut.options = options;

    if (argc - pos < 2){
        if (!node.terminal){
            exeOut.errorCode = 2;
            exeOut.errorPos = pos + 1;
            return exeOut;
        } else return node.execution(argc, argv);
    }

    auto it = node.subordinates.find(argv[pos + 1]);
    if (it == node.subordinates.end()){
        // Switch case for each type of specifier
        switch (node.specExpected){
            case Specifier::None:
                if (node.terminal){
                    exeOut.errorCode = 5;
                    exeOut.errorPos = pos + 1;
                    return exeOut;
                } else {
                    exeOut.errorCode = 1;
                    exeOut.errorPos = pos + 1;
                    return exeOut;
                }
            case Specifier::Set:
                setName = argv[pos + 1]; 
                it = node.subordinates.find("$set");
                exeOut.options.push_back({"-s, --set <SET-NAME>", "Name of the existing set"});
                break;
             case Specifier::NewSetName:
                exeOut.options.push_back({"<NEW-SET-NAME>", "Name of the new set"});
                newSetName = argv[pos + 1]; 
                it = node.subordinates.find("$newSetName");
                break;
            case Specifier::Item:
                exeOut.options.push_back({"-i, --item <INDEX>", "Index of the existing item in aforementioned set"});
                try { itemPos = std::stoi(argv[pos + 1]); }
                catch (std::logic_error) {
                    exeOut.errorCode = 1;
                    exeOut.errorPos = pos + 1;
                    return exeOut;
                }
                it = node.subordinates.find("$item");
                break;
            case Specifier::Other:
                if (node.terminal) {
                    return node.execution(argc, argv);
                } else {
                    exeOut.errorCode = -1;
                    exeOut.errorPos = pos;
                    return exeOut;
                }

            default:
                exeOut.errorCode = -1;
                exeOut.errorPos = pos + 1;
                return exeOut;
        }
        return lookUp(pos + 1, argc, argv, it->second, exeOut.options);
    } else {
        if (node.specExpected != Specifier::None){
            exeOut.errorCode = -1;
            exeOut.errorPos = pos;
            return exeOut;
        } else return lookUp(pos + 1, argc, argv, it->second, exeOut.options);
    }
}

void Command::resolveExecutingOutput(int argc, char* argv[], ExecutingOutput exeOut){
    // std::cout << exeOut.errorCode << " " << exeOut.errorPos << "\n";
    if (exeOut.errorCode == 0) return;

    std::string shortTab = "    ";

    bool discontinued = DiscontinuedMsgCode.find(exeOut.errorCode) != DiscontinuedMsgCode.end();
    std::cout << "fatal: " << ErrorCodeMessage.at(exeOut.errorCode);
    if (discontinued) std::cout << ".\n";
    else std::cout << ": `" << argv[exeOut.errorPos] << "`\n";

    std::cout << "\nUsage: \n" << shortTab << "$ quiz";
    int currentNode = 0;
    bool discontinueGuide = false;
    for (int i = 1; i < exeOut.errorPos; i++){
        Specifier previousSpec = cmdTree[currentNode].specExpected;
        std::string tmp = std::string(argv[i]);
        std::cout << ' '; 

        switch (previousSpec){
            case Specifier::None:
                if (tmp == "-s") tmp = "--set";
                if (tmp == "-i") tmp = "--item";
                std::cout << tmp;
                currentNode = cmdTree[currentNode].subordinates.find(tmp)->second;
                break;
            case Specifier::Set:
                std::cout << '<' << HelpEquivalentKeyword.at("$set") << '>';
                currentNode = cmdTree[currentNode].subordinates.find("$set")->second;
                break;
            case Specifier::Item:
                std::cout << '<' << HelpEquivalentKeyword.at("$item") << '>';
                currentNode = cmdTree[currentNode].subordinates.find("$item")->second;
                break;
            case Specifier::NewSetName:
                std::cout << '<' << HelpEquivalentKeyword.at("$newSetName") << '>';
                currentNode = cmdTree[currentNode].subordinates.find("$newSetName")->second;
                break;
            case Specifier::Other:
                std::cout << exeOut.otherspecArgumentGuide;
                discontinueGuide = true;
                break;
        }
        if (discontinueGuide) break;
    }
    if (!discontinueGuide && !cmdTree[currentNode].subordinates.empty()){
        if (cmdTree[currentNode].terminal) std::cout << " [";
        else std::cout << " <";
        bool firstEncounter = false;
        for (auto it : cmdTree[currentNode].subordinates){
            if (it.first[0] == '-' && it.first[1] != '-') continue;

            if (firstEncounter) std::cout << '|';
            else firstEncounter = true;

            if (it.first[0] == '$') std::cout << HelpEquivalentKeyword.at(it.first);
            else std::cout << it.first;
        } 
        if (cmdTree[currentNode].terminal) std::cout << "]";
        else std::cout << ">";
    }
    std::cout << "\n";

    if (exeOut.options.empty()) return;
    std::cout << "\nOptions:\n" << shortTab;
    for (auto it : exeOut.options){
        std::cout << it.first << '\n' << '\t' << it.second << '\n' << shortTab;
    }
    return;
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
        std::pair<std::string, std::string> option_s = 
            {"-s, --set <SET-NAME>", "Name of the new set"};
        exeOut.options.push_back(option_s);

        int exeCode = DataHandler.newSet(setName);
        exeOut.errorCode = exeCode;
        if (exeCode != 0) exeOut.errorPos = 3;
        else std::cout << "Set `" << setName << "` has been created.\n";
        return exeOut;
    }

    exeOut.otherspecArgumentGuide = "--front <FRONT-CONTENT> --back <BACK-CONTENT>";
    std::pair<std::string, std::string> option_s = 
        {"-s, --set <SETNAME>", "Name of the existing set"};
    std::pair<std::string, std::string> option_f = 
        {"-f, --front <CONTENT>", "Set the front content (required)"};
    std::pair<std::string, std::string> option_b = 
        {"-b, --back <CONTENT>", "Set the back content (required)"};
    exeOut.options.push_back(option_s);
    exeOut.options.push_back(option_f);
    exeOut.options.push_back(option_b);

    std::string front = "", back = "";
    bool f_arg = false, b_arg = false;
    for (int i = 4; i < argc; i++){
        if (std::string(argv[i]) == "--front" || std::string(argv[i]) == "-f"){
            f_arg = true;
            b_arg = false;
            continue;
        }

        if (std::string(argv[i]) == "--back" || std::string(argv[i]) == "-b"){
            b_arg = true;
            f_arg = false;
            continue;
        }

        if (f_arg){
            if (front != "") front += " ";
            front += argv[i];
        }   

        if (b_arg){
            if (back != "") back += " ";
            back += argv[i];
        }

        if ((!f_arg) && (!b_arg)){
            exeOut.errorCode = 1;
            exeOut.errorPos = i;
            return exeOut;
        }
    }
    if (front == "" || back == ""){
        exeOut.errorCode = 2;
        exeOut.errorPos = argc - 1;
        return exeOut;
    }
    
    if (!DataHandler.setExist(setName)){
        exeOut.errorCode = 8;
        exeOut.errorPos = 3;
        return exeOut;
    }

    if (DataHandler.cardContentExist(setName, front) != -1){
        std::string answer = "";
        std::cout << "Card front already exists. Are you sure to add the card?\n";
        while (answer == ""){
            std::cout << "[Y/N]: ";
            if (!(std::cin >> answer)) return exeOut;
            if (answer != "Y" && answer != "y" && answer != "N" && answer != "n") answer = "";
        }
        if (answer == "N" || answer == "n"){
            std::cout << "Card addition has been discarded.\n";
            return exeOut;
        }
    }

    int res = DataHandler.addCard(setName, front, back);
    exeOut.errorCode = res;
    if (res != 0) exeOut.errorPos = 3;
    else std::cout << "Card has been added.\n";
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

ExecutingOutput Command::quiz_delete_all(int argc, char* argv[]){
    return ExecutingOutput(0, 0);
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
