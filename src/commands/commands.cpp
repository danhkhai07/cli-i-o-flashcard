#include "commands/commands.h"

#include <iostream>

int Command::lookUp(int argc, const char* argv[], const int& level){
    if 
    // if 
    
}

/// @brief Add new command node
/// @param keyword 
/// @param terminal 
/// @param specExpected 
void Command::addCommandNode(std::string keyword, bool specExpected){
    CommandNode node(++nodeCount, keyword, keyword, false, specExpected, {});
    cmdTree.push_back(node);
}

/// @brief Add new command node
/// @param keyword 
/// @param dependingNode 
/// @param terminal 
/// @param specExpected 
void Command::addCommandNode(std::string keyword, int dependingNode, bool specExpected){
    CommandNode& dnp = cmdTree[dependingNode];
    CommandNode node(++nodeCount, dnp.name + "_" + keyword, keyword, false, specExpected, {});
    cmdTree.push_back(node);
    dnp.subordinates[keyword].push_back(std::ref(node));
}

/// @brief Add new command node
/// @param keyword 
/// @param dependingNode 
/// @param func 
/// @param terminal 
/// @param specExpected 
void Command::addCommandNode(std::string keyword, int dependingNode, std::function<int(int, const char*[])> func,
    bool terminal, bool specExpected){
    CommandNode& dnp = cmdTree[dependingNode];
    CommandNode node(++nodeCount, dnp.name + "_" + keyword, keyword, terminal, specExpected, func);
    cmdTree.push_back(node);
    dnp.subordinates[keyword].push_back(std::ref(node));
}