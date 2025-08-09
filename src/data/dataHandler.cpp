#include "data/dataHandler.h"

#include <vector>
#include <algorithm>

int Data::load(){
    std::ifstream in("questions.json", std::ios::in);
    if (!in) {
        dataset = nlohmann::json::object();
        return;
    }   
    try {
        dataset = nlohmann::json::parse(in);
    } catch (...) {
        return 0;
    }
    return 1;
}

int Data::save(){
    std::ofstream out("questions.json", std::ios::out | std::ios::trunc);
    out << dataset.dump(4); // pretty print
    if (!out) return 0;
    return 1;
}

int Data::newSet(std::string_view setName){
    if (!nameValid(setName)) return 1;
    if (setExist(setName)) return 2;
    dataset[setName] = {};
    save();
    return 0;
}

int Data::addCard(std::string_view setName, std::string_view front, std::string_view back){
    using namespace std::chrono; 

    if (!nameValid(setName)) return 1;
    if (!setExist(setName)) return 3; 
    if (setSize(setName) >= INT_MAX - 1) return 4;

    nlohmann::json pData;
    pData["front"] = std::string(front);
    pData["back"] = std::string(back);
    auto now = system_clock::now() + hours(7);   
    pData["lastRefresh"] = date::format("%F %T", now);
    pData["state"] = "New";
    pData["currentStep"] = 0;
    pData["easeFactor"] = 2.5;
    pData["interval"] = 0;

    dataset[setName].push_back(pData);
    save();
    return 0;
}

int Data::listSets(std::vector<std::string>& out){
    if (!out.empty()) out = {};
    for (auto it = dataset.begin(); it != dataset.end(); ++it){
        out.push_back(it.key());
    }
    return 0;
}

int Data::listCards(std::vector<nlohmann::json>& out, std::string_view setName){
    if (!out.empty()) out = {};
    if (!nameValid(setName)) return 1;
    if (!setExist(setName)) return 3; 
    for (auto it = dataset[setName].begin(); it != dataset[setName].end(); ++it){
        out.push_back(*it);
    }
    return 0;
}

bool Data::setExist(std::string_view setName){
    for (auto it = dataset.begin(); it != dataset.end(); ++it){
        if (it.key() == setName) return true;
    }
    return false;
}

// return the index (1-based) at which the card has the same front. returns -1 if no duplication
// return 0 if set doesn't exist, so check if setExist first
int Data::cardExist(std::string_view setName, std::string_view front){
    if (!setExist(setName)) return 0; 
    for (auto it = dataset[setName].begin(); it != dataset[setName].end(); ++it){
        if ((*it)["front"] == front) {
            return static_cast<int>(std::distance(dataset[setName].begin(), it)) + 1;
        }
    }
    return -1;
}

bool Data::nameValid(std::string_view name){
    bool containSpace = (name.find(' ') != std::string_view::npos);
    
    static std::vector<std::string> DisallowedNames = {
        "help", "set", "quiz", "learn", "about", "new", "kill", "rename", "list", "add", "delete",
        "$set", "$item"
    };
    bool disallowed = (std::find(DisallowedNames.begin(), DisallowedNames.end(), name) != DisallowedNames.end());

    return !(containSpace or disallowed);
}

int Data::setSize(std::string_view setName){
    if (!setExist(setName)) return 0; 
    return dataset[setName].size();
}