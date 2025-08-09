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
        void addCommandNode(std::string_view keyword, const Specifier& specExpected);
        void addCommandNode(std::string_view keyword, const Specifier& specExpected, int dependingNode);
        void addCommandNode(std::string_view keyword, const Specifier& specExpected, int dependingNode,
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
            addCommandNode("root", Specifier::None); // 0
            // 1st layer
            addCommandNode("help", Specifier::None, 0, Zeus); // 1
            addCommandNode("set", Specifier::Set, 0, Zeus); // 2
            addCommandNode("learn", Specifier::Set, 0, Zeus); // 3
            addCommandNode("about", Specifier::None, 0, Zeus); // 4
            // 2nd layer
            addCommandNode("$set", Specifier::None, 2); // 5
            addCommandNode("$set", Specifier::None, 3, Zeus); // 6
            // 3rd layer
            addCommandNode("new", Specifier::None, 5, Zeus); // 7
            addCommandNode("kill", Specifier::None, 5, Zeus); // 8
            addCommandNode("delete", Specifier::Item, 5); // 9
            addCommandNode("rename", Specifier::None, 5, Zeus); // 10
            addCommandNode("list", Specifier::None, 5, Zeus); // 11
            addCommandNode("add", Specifier::None, 5, Zeus); // 12
            addCommandNode("$item", Specifier::None, 9, Zeus); // 13
        }
        ~Command(){}

        std::pair<int, int> Command::lookUp(int pos, int argc, char* argv[], const int& nodePos);
        void errorHandling(int argc, char* argv[], int errorCode);
};