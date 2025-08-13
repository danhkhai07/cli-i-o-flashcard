#pragma once

#include "commands_export.h"
#include "data/dataHandler/dataHandler.h"

#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

class COMMANDS_SHARED Command {
    private:
        Command(const Command&);
        Command& operator=(const Command&);

    // TEMP VARIABLES (CHANGEABLE)
        // cache lookUp variables 
        std::string setName;
        int itemPos;
        std::string newSetName;

    // CORE VARIABLES & DEFINITIONS 
        Data DataHandler;

        int nodeCount = 0;
        enum Specifier {
            None = 0, Set = 1, Item = 2
        };

        struct CommandNode {
            CommandNode(
                    int n,
                    const std::string& nm,
                    const std::string& key,
                    bool term,
                    Specifier spec,
                    std::function<std::pair<int, int>(int, char*[])> func
                ) 
                : nodePos(n), name(nm), keyword(key), terminal(term), specExpected(spec), execution(func) {}

            int nodePos;
            std::string name;
            std::string keyword;
            bool terminal;
            Specifier specExpected;
            std::unordered_map<std::string, int> subordinates; 
            std::function<std::pair<int, int>(int, char*[])> execution;
        };

        std::vector<CommandNode> cmdTree;

        // OPERATING FUNCTIONS
        int addCommandNode(std::string_view keyword, const Specifier& specExpected);
        int addCommandNode(std::string_view keyword, const Specifier& specExpected, 
            std::function<std::pair<int, int>(int, char*[])> func);
        int addCommandNode(std::string_view keyword, const Specifier& specExpected, int dependingNode);
        int addCommandNode(std::string_view keyword, const Specifier& specExpected, int dependingNode,
            std::function<std::pair<int, int>(int, char*[])> func);

        void addSubordinate(const int sub, const int dependingNode);

        // EXECUTING FUNCTIONS
        std::pair<int, int> Zeus(int argc, char* argv[]);
        std::pair<int, int> quiz_help(int argc, char* argv[]);
        std::pair<int, int> quiz_about(int argc, char* argv[]);
        std::pair<int, int> quiz_new(int argc, char* argv[]);
        std::pair<int, int> quiz_learn(int argc, char* argv[]);
        std::pair<int, int> quiz_delete(int argc, char* argv[]);
        std::pair<int, int> quiz_rename(int argc, char* argv[]);
        std::pair<int, int> quiz_set(int argc, char* argv[]);
        std::pair<int, int> quiz_set_$set(int argc, char* argv[]);
        std::pair<int, int> quiz_set_$set_item_$item(int argc, char* argv[]);
    public:
        Command(){
            // Root
            int Nroot = addCommandNode("root", Specifier::None);
            // 1st layer
            int Nroot_help      = addCommandNode("help", Specifier::None, Nroot,
                [this](int argc, char* argv[]) { return Zeus(argc, argv); });
            int Nroot_about     = addCommandNode("about", Specifier::None, Nroot,
                [this](int argc, char* argv[]) { return Zeus(argc, argv); });
            int Nroot_new       = addCommandNode("new", Specifier::None, Nroot,
                [this](int argc, char* argv[]) { return Zeus(argc, argv); });
            int Nroot_learn     = addCommandNode("learn", Specifier::None, Nroot, 
                [this](int argc, char* argv[]) { return Zeus(argc, argv); });
            int Nroot_delete    = addCommandNode("delete", Specifier::None, Nroot, 
                [this](int argc, char* argv[]) { return Zeus(argc, argv); });
            int Nroot_rename    = addCommandNode("rename", Specifier::None, Nroot, 
                [this](int argc, char* argv[]) { return Zeus(argc, argv); });
            // 2nd layer
            int Nroot_set   = addCommandNode("--set", Specifier::Set, Nroot, 
                [this](int argc, char* argv[]) { return Zeus(argc, argv); });
            int Nroot_s     = addCommandNode("-s", Specifier::Set, 
                [this](int argc, char* argv[]) { return Zeus(argc, argv); });
            // 3rd layer
            int Nroot_set_$set      = addCommandNode("$set", Specifier::None, Nroot_set, 
                [this](int argc, char* argv[]) { return Zeus(argc, argv); });
            addSubordinate(Nroot_set_$set, Nroot_s);
            // 4th layer
            int Nroot_set_$set_item = addCommandNode("--item", Specifier::Item, Nroot);
            int Nroot_set_$set_i    = addCommandNode("-i", Specifier::Item, Nroot_set_$set);
            // 5th layer
            int Nroot_set_$set_item_$item = addCommandNode("$item", Specifier::None, Nroot_set_$set_item, 
                [this](int argc, char* argv[]) { return Zeus(argc, argv); });
            addSubordinate(Nroot_set_$set_item_$item, Nroot_set_$set_i);
                
        }
        ~Command(){}

        std::pair<int, int> Command::lookUp(int pos, int argc, char* argv[], const int nodePos);
};