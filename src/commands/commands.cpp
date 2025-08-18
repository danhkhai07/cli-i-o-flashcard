#include "commands/commands.h"
#include "data/card/card.h" 

#include <iostream>
#include <set>
#include <queue>
#include <algorithm>

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

static const std::string shortTab = "    ";

/// @brief A shorter way to quickly input errorCode and errorPos of an ExecutingOutput object
/// @param exeOut reference to the ExecutingOutput object
/// @param code the errorCode   
/// @param pos the errorPos
ExecutingOutput shortExeOut(ExecutingOutput& exeOut, int code, int pos){
    exeOut.errorCode = code;
    exeOut.errorPos = pos;
    return exeOut;
}

ExecutingOutput Command::lookUp(int pos, int argc, char* argv[], const int nodePos){
    CommandNode node = cmdTree[nodePos];

    if (argc - pos < 2){
        if (!node.terminal){
            return shortExeOut(exeOut, 2, pos + 1);
        } else return node.execution(argc, argv);
    }

    auto it = node.subordinates.find(argv[pos + 1]);
    if (it == node.subordinates.end()){
        // Switch case for each type of specifier
        switch (node.specExpected){
            case Specifier::None:
                if (node.terminal){
                    return shortExeOut(exeOut, 5, pos + 1);
                } else {
                    return shortExeOut(exeOut, 1, pos + 1);
                }
            case Specifier::Set:
                setName = argv[pos + 1]; 
                it = node.subordinates.find("$set");
                exeOut.options.push_back({"-s, --set <SET-NAME>", "Name of the existing set."});
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
                    return shortExeOut(exeOut, 1, pos + 1);
                }
                it = node.subordinates.find("$item");
                break;
            case Specifier::Other:
                if (node.terminal) {
                    return node.execution(argc, argv);
                } else {
                    return shortExeOut(exeOut, -1, pos);
                }

            default:
                return shortExeOut(exeOut, -1, pos + 1);
        }
        return lookUp(pos + 1, argc, argv, it->second);
    } else {
        if (node.specExpected != Specifier::None){
            return shortExeOut(exeOut, -1, pos);
        } else return lookUp(pos + 1, argc, argv, it->second);
    }
}

void Command::resolveExecutingOutput(int argc, char* argv[]){
    // std::cout << exeOut.errorCode << " " << exeOut.errorPos << "\n";
    if (exeOut.errorCode == 0) return;

    {
        bool discontinued = DiscontinuedMsgCode.find(exeOut.errorCode) != DiscontinuedMsgCode.end();
        std::cout << "Error: " << ErrorCodeMessage.at(exeOut.errorCode);
        if (discontinued) std::cout << ".\n";
        else std::cout << ": `" << argv[exeOut.errorPos] << "`\n";
    }

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
    if (!discontinueGuide && cmdTree[currentNode].specExpected == Specifier::Other){
        std::cout << ' ' << exeOut.otherspecArgumentGuide;
        discontinueGuide = true;
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

int Command::addCommandNode(std::string_view keyword, const Specifier& specExpected, int dependingNode){
    int nodePos = nodeCount;
    CommandNode node(nodePos, cmdTree[dependingNode].name + "_" + std::string(keyword), std::string(keyword), false, specExpected, nullptr);
    cmdTree.push_back(node);
    cmdTree[dependingNode].subordinates[std::string(keyword)] = nodePos;
    nodeCount++;
    return nodePos;
}

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

ExecutingOutput Command::quiz_version(int argc, char* argv[]){
    std::cout << "CLI-IO-Flashcard 1.0.0\n";
    return exeOut;
}

ExecutingOutput Command::quiz_help(int argc, char* argv[]){
    std::cout << "CLI-IO-Flashcard Help\n\n";
    std::cout << "Usage:\n";
    std::cout << shortTab << "$ quiz <command> [options]\n\n";

    std::cout << "Available commands:\n";
    std::cout << shortTab << "-v, --version     Show the current version.\n";
    std::cout << shortTab << "about             Show information about this program.\n";
    std::cout << shortTab << "help              Show this help message.\n";
    std::cout << shortTab << "--set             List all sets and their status.\n";
    std::cout << shortTab << "--set <SET-NAME>\n";
    std::cout << shortTab << shortTab << "Show cards in a set.\n";
    std::cout << shortTab << "--set <SET-NAME> --item <INDEX>\n";
    std::cout << shortTab << shortTab << "Show details of a specific card.\n";
    std::cout << shortTab << "new --set <SET-NAME>\n";
    std::cout << shortTab << shortTab << "Create a new set.\n";
    std::cout << shortTab << "new --set <SET-NAME> --front \"<FRONT-CONTENT>\" --back \"<BACK-CONTENT>\"\n";
    std::cout << shortTab << shortTab << "Create a new card in specified set.\n";
    std::cout << shortTab << "learn --set <SET-NAME>\n";
    std::cout << shortTab << shortTab << "Start a learning session on the set.\n";
    std::cout << shortTab << "learn --set <SET-NAME> --item <INDEX>\n";
    std::cout << shortTab << shortTab << "Learn a specific card.\n";
    std::cout << shortTab << "delete --all        Delete ALL sets and cards.\n";
    std::cout << shortTab << "delete --set <SET-NAME>\n";
    std::cout << shortTab << shortTab << "Delete a specific set.\n";
    std::cout << shortTab << "delete --set <SET-NAME> --item <INDEX>\n";
    std::cout << shortTab << shortTab << "Delete a specific card.\n";
    std::cout << shortTab << "rename --set <SET-NAME> <NEW-SET-NAME>\n";
    std::cout << shortTab << shortTab << "Rename a set.\n\n";

    std::cout << "Examples:\n";
    std::cout << shortTab << "$ quiz new --set French\n";
    std::cout << shortTab << "$ quiz new --set French --front \"Bonjour\" --back \"Hello\"\n";
    std::cout << shortTab << "$ quiz learn --set French\n";
    std::cout << shortTab << "$ quiz delete --set French --item 0\n";

    return exeOut;
}

ExecutingOutput Command::quiz_about(int argc, char* argv[]){
    std::cout << "Developed by danhkhai07\n";
    std::cout << "Check out the whole repo at: https://github.com/danhkhai07/cli-io-flashcard\n";
    return exeOut;
}

ExecutingOutput Command::quiz_new_set_$set(int argc, char* argv[]){
    // std::cout << "Reached.\n";
    if (argc <= 4){
        int exeCode = DataHandler.newSet(setName);
        exeOut.errorCode = exeCode;
        if (exeCode != 0) exeOut.errorPos = 3;
        else std::cout << "Set `" << setName << "` has been created.\n";
        return exeOut;
    }

    exeOut.otherspecArgumentGuide = "--front \"<FRONT-CONTENT>\" --back \"<BACK-CONTENT>\"";
    std::pair<std::string, std::string> option_f = 
        {"-f, --front \"<CONTENT>\"", "Set the front content. (required)"};
    std::pair<std::string, std::string> option_b = 
        {"-b, --back \"<CONTENT>\"", "Set the back content. (required)"};
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
            return shortExeOut(exeOut, 1, i);
        }
    }
    if (front == "" || back == ""){
        return shortExeOut(exeOut, 2, argc - 1);
    }
    
    if (!DataHandler.setExist(setName)){
        return shortExeOut(exeOut, 8, 3);
    }

    if (DataHandler.cardContentExist(setName, front) != -1){
        std::string answer = "";
        std::cout << "Card front already exists. Are you sure to add the card?\n";
        while (answer == ""){
            std::cout << "[Y/N] ";
            if (!(std::getline(std::cin, answer))){
                std::cerr << "Bad input.\n";
                return exeOut;
            }
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

Card Command::learnCard(const Card& card, bool& quit_flag){

    auto printFront = [card](){
        std::cout << "Front: " << card.front << '\n';
    };

    auto printBack = [card](){
        std::cout << "Back: " << card.back << '\n';
    };

    auto convertTime = [card](Grade grade){
        if (card.gradeTimeIntervals.at(grade) < 1){
            return std::to_string(static_cast<long>(card.gradeTimeIntervals.at(grade)*1440)) + "m";
        }
        return std::to_string(static_cast<long>(card.gradeTimeIntervals.at(grade))) + "d";
    };
    
    auto printOptions = [card, convertTime](){
        std::cout << "Choose an option:\n";
        std::cout << shortTab << "[1] Again -> " << convertTime(Grade::Again) << '\n';
        std::cout << shortTab << "[2] Hard  -> " << convertTime(Grade::Hard) << '\n';
        std::cout << shortTab << "[3] Good  -> " << convertTime(Grade::Good) << '\n';
        std::cout << shortTab << "[4] Easy  -> " << convertTime(Grade::Easy) << '\n';
    };

    auto printReveal = [card, printFront, printBack, printOptions](){
        printFront();
        printBack();
        printOptions();
    };

    static auto printHelp = [](){
            std::cout << "Available commands:\n";
            std::cout << shortTab << "Enter (auto-mode)   Reveal the card before revealed, auto-choose Good after revealed\n";
            std::cout << shortTab << "reveal / r          Reveal the card's back\n";
            std::cout << shortTab << "1                   Grade: Again (after reveal)\n";
            std::cout << shortTab << "2                   Grade: Hard (after reveal)\n";
            std::cout << shortTab << "3                   Grade: Good (after reveal)\n";
            std::cout << shortTab << "4                   Grade: Easy (after reveal)\n";
            std::cout << shortTab << "skip / s            Skip this card (only if not revealed)\n";
            std::cout << shortTab << "next / n            Go to next card (after reveal)\n";
            std::cout << shortTab << "bubble / b          Reprint the current card and options\n";
            std::cout << shortTab << "help / h            Show this help message\n";
            std::cout << shortTab << "quit / q            Quit the learning session\n";
    };

    static const std::map<std::string, Grade> optionToGrade = {
        {"1", Grade::Again},
        {"2", Grade::Hard},
        {"3", Grade::Good},
        {"4", Grade::Easy}
    };

    enum class LearnOption {
        GRADE, QUIT, SKIP, NEXT, REVEAL, BUBBLE, HELP, AUTO
    };

    static const std::map<std::string, LearnOption> LearnOptionMap = {
        {"1", LearnOption::GRADE}, {"2", LearnOption::GRADE},
        {"3", LearnOption::GRADE}, {"4", LearnOption::GRADE},

        {"quit", LearnOption::QUIT}, {"q", LearnOption::QUIT},

        {"skip", LearnOption::SKIP}, {"s", LearnOption::SKIP},

        {"next", LearnOption::NEXT}, {"n", LearnOption::NEXT},

        {"reveal", LearnOption::REVEAL}, {"r", LearnOption::REVEAL},

        {"bubble", LearnOption::BUBBLE}, {"b", LearnOption::BUBBLE},

        {"help", LearnOption::HELP}, {"h", LearnOption::HELP},

        {"", LearnOption::AUTO}
    };

    Card tmp = card;
    bool revealed = false;
    bool break_flag = false;

    printFront();
    while (!break_flag){
        std::string input = "";
        std::cout << ">> ";
        if (!std::getline(std::cin, input)){
            std::cerr << "Bad input.\n";
            quit_flag = true;
            break;
        }

        auto it = LearnOptionMap.find(input);
        if (it == LearnOptionMap.end()){
            std::cout << "Unknown command. Type `help` to show available commands.\n";
            continue;
        }

        LearnOption option = LearnOptionMap.at(input);
        switch (option){
            case LearnOption::AUTO:
                if (revealed){
                    break_flag = true;
                    tmp.review(Grade::Good);
                    break;
                }
            case LearnOption::REVEAL:
                revealed = true;
                printReveal();
                break;
            case LearnOption::GRADE:
                if (!revealed){
                    std::cout << "This command is currently unavailable.\n";
                    break;
                }
                break_flag = true;
                tmp.review(optionToGrade.at(input));
                break;
            case LearnOption::QUIT:
                quit_flag = true;
                break_flag = true;
                break;
            case LearnOption::SKIP:
                break_flag = true;
                if (revealed){
                    std::cout << "Cannot skip if card is revealed. Please use `next` to continue to the next card.\n";
                    break;
                }
                tmp.review(Grade::Again);
                break;
            case LearnOption::NEXT:
                if (!revealed){
                    std::cout << "This command is currently unavailable.\n";
                    break;
                }
                break_flag = true;
                break;
            case LearnOption::BUBBLE:
                printFront();
                if (revealed){
                    printBack();
                    printOptions();
                }
                break;
            case LearnOption::HELP:
                printHelp();
                break;
        }
    }
    return tmp;
}

ExecutingOutput Command::quiz_learn_set_$set(int argc, char* argv[]){
    if (!DataHandler.setExist(setName)) return shortExeOut(exeOut, 8, 3);
    if (DataHandler.setSize(setName) == 0){
        std::cout << "Set `" << setName << "` is empty.\n";
        std::cout << "Use: $ quiz new --set " << setName << " --front \"<FRONT-CONTENT>\" --back \"<BACK-CONTENT>\"\n";
        std::cout << shortTab << "to create a new card.\n";
    }

    std::vector<Card> cardset;
    {   // declaring scope of 'out'
        int out = DataHandler.getSet(cardset, setName);
        if (out != 0) return shortExeOut(exeOut, out, 3);
    }

    for (size_t i = 0; i < cardset.size(); i++){
        if (!cardset[i].due()) cardset.erase(cardset.begin() + i);
    }
    std::make_heap(cardset.begin(), cardset.end());

    Card tmpCard;
    bool quit_flag = false;
    std::cout << "Use `help` to show available commands.\n";
    while (!cardset.empty()){
        tmpCard = cardset.front();
        if (tmpCard.interval >= 1) break;

        tmpCard = learnCard(tmpCard, quit_flag);
        if (quit_flag) break;
        
        std::pop_heap(cardset.begin(), cardset.end());
        cardset.pop_back();

        cardset.push_back(tmpCard);
        std::push_heap(cardset.begin(), cardset.end());
    }
    if (!quit_flag){
        std::cout << "Congratulations! You have finished set `" << setName << "` for today.\n";
    }

    DataHandler.writeSet(cardset, setName);

    return exeOut;
}

ExecutingOutput Command::quiz_learn_set_$set_item_$item(int argc, char* argv[]){
    // std::cout << "Reached.\n";
    return ExecutingOutput(0, 0, "");
}

ExecutingOutput Command::quiz_delete_all(int argc, char* argv[]){
    std::string answer = "";
    std::cout << "This action will wipe all data. Are you sure to proceed?\n";
    while (answer == ""){
        std::cout << "[Y/N] ";
        if (!(std::getline(std::cin, answer))){
            std::cerr << "Bad input.\n";
            return exeOut;
        }
        if (answer != "Y" && answer != "y" && answer != "N" && answer != "n") answer = "";
    }
    if (answer == "N" || answer == "n"){
        std::cout << "Data flush has been discarded.\n";
        return exeOut;
    }

    DataHandler.flushFile();
    std::cout << "All data has been wiped.\n";

    return exeOut;
}

ExecutingOutput Command::quiz_delete_set_$set(int argc, char* argv[]){
    if (!DataHandler.setExist(setName)) return shortExeOut(exeOut, 8, 3);

    DataHandler.killSet(setName);
    std::cout << "Set `" << setName << "` has been deleted.\n";

    return exeOut;
}

ExecutingOutput Command::quiz_delete_set_$set_item_$item(int argc, char* argv[]){
    if (!DataHandler.setExist(setName)) return shortExeOut(exeOut, 8, 3);
    if (!DataHandler.cardIdxExist(setName, itemPos)) return shortExeOut(exeOut, 10, 5);

    DataHandler.deleteCard(setName, itemPos);
    std::cout << "Card [" << itemPos << "] of set `" << setName << "` has been deleted.\n";

    return exeOut;
}

ExecutingOutput Command::quiz_rename_set_$set_$newSetName(int argc, char* argv[]){
    if (!DataHandler.setExist(setName)) return shortExeOut(exeOut, 8, 3);
    if (!DataHandler.nameValid(newSetName)) return shortExeOut(exeOut, 6, 4);

    DataHandler.renameSet(setName, newSetName);
    std::cout << "Set `" << setName << "` has been renamed to `" << newSetName << "`.\n";

    return exeOut;
}

ExecutingOutput Command::quiz_set(int argc, char* argv[]){
    std::vector<std::string> out;
    DataHandler.listSets(out);

    if (out.empty()){
        std::cout << "No set has been created yet. Use the following commands to start.\n";
        std::cout << "Use: $ quiz new --set <SET-NAME>\n";
        std::cout << shortTab << "to create a new set.\n";
        std::cout << "Use: $ quiz new --set <SET-NAME> --front \"<FRONT-CONTENT>\" --back \"<BACK-CONTENT>\"\n";
        std::cout << shortTab << "to create a new card.\n";
        return exeOut;
    }
   
    auto printSetStatus = [this, out](size_t idx){
        if (idx < 0 && idx >= out.size()){
            std::cerr << "printSetStatus: Bad access ([" << idx << "] on dataset.\n";
            return;
        }
        std::cout << "Set `" << out[idx] << "`:\n";
        std::cout << shortTab << "- Total card(s): " << DataHandler.setSize(out[idx]) << '\n';
        std::cout << shortTab << "- Card(s) due today: " << DataHandler.dueToday(out[idx]) << '\n';
    };

    const int preview = 1;
    for (size_t i = 0; i < preview; i++){
        std::cout << '\n';
        printSetStatus(i);
    }

    if (out.size() <= preview){
        return exeOut;
    }

    size_t count = preview;
    std::string answer = "";
    std::cout << "\nPress Enter to see remaining set(s) (one by one)\n";
    std::cout << "Press `all` to see all remaining set(s) (all at once)\n";
    std::cout << "Press `q` to quit.\n";
    while (answer == "" && count < out.size()){
        std::cout << ":";
        if (!(std::getline(std::cin, answer))){
            std::cerr << "Bad input.\n";
            return exeOut;
        }

        // delete ":"
        std::cout << "\033[A\033[2K";

        if (answer != "" && answer != "q" && answer != "all"){
            answer = "";
            continue;
        }
        if (answer == "q") break;
        if (answer == "all"){
            if (count == preview) std::cout << "\033[A\033[2K\033[A\033[2K\033[A\033[2K\033[A";
            while (count < out.size()){
                std::cout << '\n';
                printSetStatus(count++);
            }
            break;
        }

        // delete the guide lines (Press Enter to...)
        if (count == preview) std::cout << "\033[A\033[2K\033[A\033[2K\033[A\033[2K\033[A";
        
        std::cout << '\n';
        printSetStatus(count++);
    }

    return exeOut;
}

ExecutingOutput Command::quiz_set_$set(int argc, char* argv[]){
    if (!DataHandler.setExist(setName)) return shortExeOut(exeOut, 8, 3);

    std::vector<Card> out;
    DataHandler.listCards(out, setName);

    if (out.empty()){
        std::cout << "There is no card in set`" << setName << "`. Use the following command to start.\n";
        std::cout << "Use: $ quiz new --set <SET-NAME> --front \"<FRONT-CONTENT>\" --back \"<BACK-CONTENT>\"\n";
        std::cout << shortTab << "to create a new card.\n";
        return exeOut;
    }

    auto printCardStatus = [this, &out](size_t idx){
        if (idx < 0 && idx >= out.size()){
            std::cerr << "printCardStatus: Bad access ([" << idx << "] on set`" << setName << "`.\n";
            return;
        }
        
        using namespace std::chrono;
        std::cout << "[" << idx << "]:\n";
        Card tmpCard = out[idx];
        std::cout << shortTab << "- Front: " << tmpCard.front << '\n';
        std::cout << shortTab << "- Back: " << tmpCard.back << '\n';
        std::cout << shortTab << "- isDue: " << (tmpCard.due() ? "true" : "false") << '\n';

        std::istringstream lastIn(tmpCard.lastRefresh);
        date::sys_time<seconds> last;
        lastIn >> date::parse("%F %T", last);
        last += seconds(static_cast<long>(tmpCard.interval*86400));
        auto now = system_clock::now() + hours(7);
        std::cout << shortTab << "- Next review: " << date::format("%F", (tmpCard.due() ? now :last)) << '\n';
    };

    const int preview = 1;
    for (size_t i = 0; i < preview; i++){
        std::cout << '\n';
        printCardStatus(i);
    }

    if (out.size() <= preview){
        return exeOut;
    }

    size_t count = preview;
    std::string answer = "";
    std::cout << "\nPress Enter to see remaining card(s) (one by one)\n";
    std::cout << "Press `all` to see all remaining card(s) (all at once)\n";
    std::cout << "Press `q` to quit.\n";
    while (answer == "" && count < out.size()){
        std::cout << ":";
        if (!(std::getline(std::cin, answer))){
            std::cerr << "Bad input.\n";
            return exeOut;
        }

        // delete ":"
        std::cout << "\033[A\033[2K";

        if (answer != "" && answer != "q" && answer != "all"){
            answer = "";
            continue;
        }
        if (answer == "q") break;
        if (answer == "all"){
            if (count == preview) std::cout << "\033[A\033[2K\033[A\033[2K\033[A\033[2K\033[A";
            while (count < out.size()){
                std::cout << '\n';
                printCardStatus(count++);
            }
            break;
        }

        // delete the guide lines (Press Enter to...)
        if (count == preview) std::cout << "\033[A\033[2K\033[A\033[2K\033[A\033[2K\033[A";
        
        std::cout << '\n';
        printCardStatus(count++);
    }


    return exeOut;
}

ExecutingOutput Command::quiz_set_$set_item_$item(int argc, char* argv[]){
    if (!DataHandler.setExist(setName)) return shortExeOut(exeOut, 8, 3);
    if (!DataHandler.cardIdxExist(setName, itemPos)) return shortExeOut(exeOut, 10, 5);

    using namespace std::chrono;
    std::cout << "[" << itemPos<< "]:\n";
    Card tmpCard;
    DataHandler.getCard(tmpCard, setName, itemPos);
    std::cout << shortTab << "- Front: " << tmpCard.front << '\n';
    std::cout << shortTab << "- Back: " << tmpCard.back << '\n';
    std::cout << shortTab << "- isDue: " << (tmpCard.due() ? "true" : "false") << '\n';

    std::istringstream lastIn(tmpCard.lastRefresh);
    date::sys_time<seconds> last;
    lastIn >> date::parse("%F %T", last);
    last += seconds(static_cast<long>(tmpCard.interval*86400));
    auto now = system_clock::now() + hours(7);
    std::cout << shortTab << "- Next review: " << date::format("%F", (tmpCard.due() ? now :last)) << '\n';

    return exeOut;
}