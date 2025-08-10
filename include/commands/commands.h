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
        int addCommandNode(std::string_view keyword, const Specifier& specExpected, int dependingNode);
        int addCommandNode(std::string_view keyword, const Specifier& specExpected, int dependingNode,
            std::function<std::pair<int, int>(int, char*[])> func);

        // EXECUTING FUNCTIONS
        static std::pair<int, int> Zeus(int argc, char* argv[]);
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
            int Nroot_help     = addCommandNode("help", Specifier::None, Nroot, Zeus);
            int Nroot_set      = addCommandNode("set", Specifier::Set, Nroot, Zeus);
            int Nroot_learn    = addCommandNode("learn", Specifier::Set, Nroot, Zeus);
            int Nroot_about    = addCommandNode("about", Specifier::None, Nroot, Zeus);
            // 2nd layer
            int Nroot_set_$set     = addCommandNode("$set", Specifier::None, Nroot_set);
            int Nroot_learn_$set   = addCommandNode("$set", Specifier::None, Nroot_set, Zeus);
            // 3rd layer
            int Nroot_set_$set_new     = addCommandNode("new", Specifier::None, Nroot_set_$set, Zeus);
            int Nroot_set_$set_kill    = addCommandNode("kill", Specifier::None, Nroot_set_$set, Zeus); 
            int Nroot_set_$set_delete  = addCommandNode("delete", Specifier::Item, Nroot_set_$set); 
            int Nroot_set_$set_rename  = addCommandNode("rename", Specifier::None, Nroot_set_$set, Zeus); 
            int Nroot_set_$set_list    = addCommandNode("list", Specifier::None, Nroot_set_$set, Zeus);
            int Nroot_set_$set_add     = addCommandNode("add", Specifier::None, Nroot_set_$set, Zeus); 
            // 4th layer
            int Nroot_set_$set_delete_$item    = addCommandNode("$item", Specifier::None, Nroot_set_$set_delete, Zeus); 
        }
        ~Command(){}

        std::pair<int, int> Command::lookUp(int pos, int argc, char* argv[], const int nodePos);
};