#include "commands/commands.h"

#include <iostream>

void Command::help(int argc, const char* argv[]){
    std::cout << "'help' reached.";
}

void Command::item(int argc, const char* argv[]){
    std::cout << "'item' reached.\n";
    if (std::string(argv[2]) == "add"){
        itemAdd(argc, argv);
    } 
    else if (std::string(argv[2]) == "list"){
        itemList(argc, argv);
    }
}

void Command::learn(int argc, const char* argv[]){
    std::cout << "'learn' reached.";
}

void Command::itemAdd(int argc, const char* argv[]){
    std::cout << "'item add' reached.";
}

void Command::itemList(int argc, const char* argv[]){
    std::cout << "'item list' reached.";
}