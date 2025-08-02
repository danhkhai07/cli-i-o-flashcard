#pragma once

#include <functional>
#include <string>

class Command {
    public:
        Command(){
        //    addCommandNode("root");
        }
        ~Command(){}

        int lookUp(int argc, const char* argv[], const int& level);
    private:
        Command(const Command&);
        Command& operator=(const Command&);

        int nodeCount = 0;

        struct CommandNode {
            CommandNode(){};
            CommandNode(
                    int n,
                    std::string nm,
                    std::string key,
                    bool term,
                    bool spec,
                    std::function<int(int, const char*[])> func
                ) 
                : node(n), name(nm), keyword(key), terminal(term), specifierExpected(spec), execution(func) {}

            int node;
            std::string name;
            std::string keyword;
            bool terminal;
            bool specifierExpected;
            std::unordered_map<std::string, std::vector<std::reference_wrapper<CommandNode>>> subordinates; 
            std::function<int(int, const char*[])> execution;
        };

        std::vector<CommandNode> cmdTree;

    // OPERATING FUNCTIONS
        void addCommandNode(std::string keyword, bool specExpected);
        void addCommandNode(std::string keyword, int dependingNode, bool specExpected);
        void addCommandNode(std::string keyword, int dependingNode, std::function<int(int, const char*[])> func,
            bool terminal, bool specExpected);

    // EXECUTING FUNCTIONS
};