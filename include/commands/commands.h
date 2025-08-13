#pragma once

#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

class Command {
    private:
        Command(const Command&);
        Command& operator=(const Command&);

    // TEMP VARIABLES (CHANGEABLE)
        // cache lookUp variables 
        std::string setName;
        int itemPos;
        std::string newSetName;

    // CORE VARIABLES & DEFINITIONS 
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

        // EXECUTING FUNCTIONS
        std::pair<int, int> Zeus(int argc, char* argv[]);
        std::pair<int, int> quiz_help(int argc, char* argv[]);
        std::pair<int, int> quiz_set(int argc, char* argv[]);
        std::pair<int, int> quiz_learn(int argc, char* argv[]);
        std::pair<int, int> quiz_about(int argc, char* argv[]);
        std::pair<int, int> quiz_set_new(int argc, char* argv[]);
        std::pair<int, int> quiz_set_rename(int argc, char* argv[]);
        std::pair<int, int> quiz_set_kill(int argc, char* argv[]);
        std::pair<int, int> quiz_set_list(int argc, char* argv[]);
        std::pair<int, int> quiz_set_add(int argc, char* argv[]);
        std::pair<int, int> quiz_set_delete(int argc, char* argv[]);

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
        }
        ~Command(){}

        std::pair<int, int> Command::lookUp(int pos, int argc, char* argv[], const int nodePos);
};