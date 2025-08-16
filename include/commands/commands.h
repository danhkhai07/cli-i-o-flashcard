#pragma once

#include "commands_export.h"
#include "data/dataHandler/dataHandler.h"

#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

struct COMMANDS_SHARED ExecutingOutput {
    ExecutingOutput(){}
    ExecutingOutput(
        int err,
        int pos,
        std::string_view guide
    ): errorCode(err), errorPos(pos), otherspecArgumentGuide(guide) {}

    ExecutingOutput(
        int err,
        int pos
    ): errorCode(err), errorPos(pos) {}

    int errorCode = 0;
    int errorPos = 0;
    std::string otherspecArgumentGuide = "";
    std::vector<std::pair<std::string, std::string>> options;
};

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
        ExecutingOutput exeOut;

        int nodeCount = 0;
        enum Specifier {
            None, Set, Item, NewSetName, Other
        };

        struct CommandNode {
            CommandNode(
                    int n,
                    const std::string& nm,
                    const std::string& key,
                    bool term,
                    Specifier spec,
                    std::function<ExecutingOutput(int, char*[])> func
                ) 
                : nodePos(n), name(nm), keyword(key), terminal(term), specExpected(spec), execution(func) {}

            int nodePos;
            std::string name;
            std::string keyword;
            bool terminal;
            Specifier specExpected;
            std::unordered_map<std::string, int> subordinates; 
            std::function<ExecutingOutput(int, char*[])> execution;
        };

        std::vector<CommandNode> cmdTree;

        // OPERATING FUNCTIONS
        int addCommandNode(std::string_view keyword, const Specifier& specExpected);
        int addCommandNode(std::string_view keyword, const Specifier& specExpected, 
            std::function<ExecutingOutput(int, char*[])> func);
        int addCommandNode(std::string_view keyword, const Specifier& specExpected, int dependingNode);
        int addCommandNode(std::string_view keyword, const Specifier& specExpected, int dependingNode,
            std::function<ExecutingOutput(int, char*[])> func);

        void addSubordinate(const int sub, const int dependingNode);

        // UTILITY FUNCTIONS
        Card learnCard(const Card& card);

        // EXECUTING FUNCTIONS
        ExecutingOutput Zeus(int argc, char* argv[]);
        ExecutingOutput quiz_help(int argc, char* argv[]);
        ExecutingOutput quiz_about(int argc, char* argv[]);
        ExecutingOutput quiz_new_set_$set(int argc, char* argv[]);
        ExecutingOutput quiz_learn_set_$set(int argc, char* argv[]);
        ExecutingOutput quiz_learn_set_$set_item_$item(int argc, char* argv[]);
        ExecutingOutput quiz_delete_all(int argc, char* argv[]);
        ExecutingOutput quiz_delete_set_$set(int argc, char* argv[]);
        ExecutingOutput quiz_delete_set_$set_item_$item(int argc, char* argv[]);
        ExecutingOutput quiz_rename_set_$set_$newSetName(int argc, char* argv[]);
        ExecutingOutput quiz_set(int argc, char* argv[]);
        ExecutingOutput quiz_set_$set(int argc, char* argv[]);
        ExecutingOutput quiz_set_$set_item_$item(int argc, char* argv[]);
    public:
        Command(int argc, char* argv[]){
            // Assign DataHandler
            DataHandler = Data(argc, argv);
            
            // Root
            int root = addCommandNode("root", Specifier::None,
                [this](int argc, char* argv[]) { return quiz_help(argc, argv); });
            
            // 1st layer
            int root_help      = addCommandNode("help", Specifier::None, root,
                [this](int argc, char* argv[]) { return quiz_help(argc, argv); });
            int root_about     = addCommandNode("about", Specifier::None, root,
                [this](int argc, char* argv[]) { return quiz_about(argc, argv); });
            int root_new       = addCommandNode("new", Specifier::None, root);
            int root_learn     = addCommandNode("learn", Specifier::None, root);
            int root_delete    = addCommandNode("delete", Specifier::None, root);
            int root_rename    = addCommandNode("rename", Specifier::None, root);
            int root_set   = addCommandNode("--set", Specifier::Set, root, 
                [this](int argc, char* argv[]) { return quiz_set(argc, argv); });
            int root_s     = addCommandNode("-s", Specifier::Set, root,
                [this](int argc, char* argv[]) { return quiz_set(argc, argv); });

            // 2nd layer
            int root_set_$set  = addCommandNode("$set", Specifier::None, root_set, 
                [this](int argc, char* argv[]) { return quiz_set_$set(argc, argv); });
                addSubordinate(root_set_$set, root_s);
            int root_new_set   = addCommandNode("--set", Specifier::Set, root_new);
            int root_new_s     = addCommandNode("-s", Specifier::Set, root_new);
            int root_learn_set = addCommandNode("--set", Specifier::Set, root_learn);
            int root_learn_s   = addCommandNode("-s", Specifier::Set, root_learn);
            int root_delete_set = addCommandNode("--set", Specifier::Set, root_delete);
            int root_delete_s   = addCommandNode("-s", Specifier::Set, root_delete);
            int root_delete_all     = addCommandNode("--all", Specifier::None, root_delete, 
                [this](int argc, char* argv[]) { return quiz_delete_all(argc, argv); });
            int root_delete_a     = addCommandNode("-a", Specifier::None, root_delete, 
                [this](int argc, char* argv[]) { return quiz_delete_all(argc, argv); });
            int root_rename_set = addCommandNode("--set", Specifier::Set, root_rename);
            int root_rename_s   = addCommandNode("-s", Specifier::Set, root_rename);
            
            // 3rd layer
            int root_set_$set_item = addCommandNode("--item", Specifier::Item, root_set_$set);
            int root_set_$set_i    = addCommandNode("-i", Specifier::Item, root_set_$set);
            int root_new_set_$set  = addCommandNode("$set", Specifier::Other, root_new_set, 
                [this](int argc, char* argv[]) { return quiz_new_set_$set(argc, argv); });
                addSubordinate(root_new_set_$set, root_new_s);
            int root_learn_set_$set  = addCommandNode("$set", Specifier::None, root_learn_set, 
                [this](int argc, char* argv[]) { return quiz_learn_set_$set(argc, argv); });
                addSubordinate(root_learn_set_$set, root_learn_s);
            int root_delete_set_$set  = addCommandNode("$set", Specifier::None, root_delete_set, 
                [this](int argc, char* argv[]) { return quiz_delete_set_$set(argc, argv); });
                addSubordinate(root_delete_set_$set, root_delete_s);
            int root_rename_set_$set  = addCommandNode("$set", Specifier::NewSetName, root_rename_set);
                addSubordinate(root_rename_set_$set, root_rename_s);

            // 4th layer
            int root_set_$set_item_$item = addCommandNode("$item", Specifier::None, root_set_$set_item, 
                [this](int argc, char* argv[]) { return quiz_set_$set_item_$item(argc, argv); });
                addSubordinate(root_set_$set_item_$item, root_set_$set_i);
            int root_learn_set_$set_item = addCommandNode("--item", Specifier::Item, root_learn_set_$set);
            int root_learn_set_$set_i    = addCommandNode("-i", Specifier::Item, root_learn_set_$set);
            int root_delete_set_$set_item = addCommandNode("--item", Specifier::Item, root_delete_set_$set);
            int root_delete_set_$set_i    = addCommandNode("-i", Specifier::Item, root_delete_set_$set);
            int root_rename_set_$set_$newSetName = addCommandNode("$newSetName", Specifier::None, root_rename_set_$set,
                [this](int argc, char* argv[]) { return quiz_rename_set_$set_$newSetName(argc, argv); });

            // 5th layer
            int root_learn_set_$set_item_$item = addCommandNode("$item", Specifier::None, root_learn_set_$set_item,
                [this](int argc, char* argv[]) { return quiz_learn_set_$set_item_$item(argc, argv); });
                addSubordinate(root_learn_set_$set_item_$item, root_learn_set_$set_i);
            int root_delete_set_$set_item_$item = addCommandNode("$item", Specifier::None, root_delete_set_$set_item,
                [this](int argc, char* argv[]) { return quiz_delete_set_$set_item_$item(argc, argv); });
                addSubordinate(root_delete_set_$set_item_$item, root_delete_set_$set_i);
                
        }
        ~Command(){}

        ExecutingOutput Command::lookUp(int pos, int argc, char* argv[], const int nodePos);
        void resolveExecutingOutput(int argc, char* argv[]);
};