#pragma once

#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

class Command {
    public:
        Command(){
            // Root
            addCommandNode("root", Specifier::None);
            // 1st layer
            addCommandNode("help", Specifier::None, 0, Zeus);
            addCommandNode("set", Specifier::Set, 0, Zeus);
            addCommandNode("learn", Specifier::Set, 0, Zeus);
            addCommandNode("about", Specifier::None, 0, Zeus);
            // 2nd layer
            addCommandNode("new", Specifier::None, 2, Zeus);
            addCommandNode("kill", Specifier::None, 2, Zeus);
            addCommandNode("delete", Specifier::Item, 2, Zeus);
            addCommandNode("rename", Specifier::None, 2, Zeus);
            addCommandNode("list", Specifier::None, 2, Zeus);
            addCommandNode("add", Specifier::None, 2, Zeus);
        }
        ~Command(){}

        std::pair<int, int> Command::lookUp(int pos, int argc, char* argv[], const int& nodePos);
        void errorHandling(int argc, char* argv[], int errorCode);
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
                    const int& n,
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
        void addCommandNode(const std::string& keyword, const Specifier& specExpected);
        void addCommandNode(const std::string& keyword, const Specifier& specExpected, int dependingNode);
        void addCommandNode(const std::string& keyword, const Specifier& specExpected, int dependingNode,
            std::function<std::pair<int, int>(int, char*[])> func);

    // EXECUTING FUNCTIONS
        static std::pair<int, int> Zeus(int argc, char* argv[]);
};