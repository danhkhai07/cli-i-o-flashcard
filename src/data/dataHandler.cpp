#include "data/dataHandler.h"

#include <vector>
#include <algorithm>

int Data::load(){
    std::ifstream in("questions.json", std::ios::in);
    if (!in) {
        dataset = nlohmann::json::object();
        return 0;
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
    out << dataset.dump(4);
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

int Data::killSet(std::string_view setName){
    if (!setExist(setName)) return 3;

    dataset.erase(setName);
    save();
    return 0;
}

int Data::deleteCard(std::string_view setName, const int cardPos){
    if (!setExist(setName)) return 3;
    if (!cardIdxExist(setName, cardPos)) return 5;
    
    dataset[setName].erase(dataset[setName].begin() + cardPos);
    save();
    return 0;
}

int Data::renameSet(std::string_view setName, std::string_view newName){
    if (!setExist(setName)) return 3;
    if (!nameValid(newName)) return 1;

    dataset[newName] = dataset[setName];
    dataset.erase(setName);
    save();
    return 0;
}

void Data::flushFile(){
    dataset = nlohmann::json::object();
    save();
    return;
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

int Data::cardContentExist(std::string_view setName, std::string_view front){
    if (!setExist(setName)) return 0; 
    for (auto it = dataset[setName].begin(); it != dataset[setName].end(); ++it){
        if ((*it)["front"] == std::string(front)) {
            return static_cast<int>(std::distance(dataset[setName].begin(), it)) + 1;
        }
    }
    return -1;
}

int Data::cardIdxExist(std::string_view setName, const int idx){
    if (!setExist(setName)) return 0;
    if (idx < 0 || idx >= setSize(setName)) return 0;
    return 1;
}

bool Data::nameValid(std::string_view name){
    bool containSpace = (name.find(' ') != std::string_view::npos);
    
    static std::vector<std::string> DisallowedNames = {
        "help", "set", "quiz", "learn", "about", "new", "kill", "rename", "list", "add", "delete",
        "$set", "$item"
    };
    bool disallowed = (std::find(DisallowedNames.begin(), DisallowedNames.end(), name) != DisallowedNames.end());

    return !(containSpace || disallowed);
}

int Data::setSize(std::string_view setName){
    if (!setExist(setName)) return 0; 
    return static_cast<int>(dataset[setName].size());
}