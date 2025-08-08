#include "data/dataHandler.h"

#include <vector>
#include <algorithm>

void Data::load(){
    if (!iquestions.is_open()) open_io_gate();
    dataset = nlohmann::json::parse(iquestions);
}

void Data::open_io_gate(){
    if (!iquestions.is_open()) iquestions.close();
    if (!oquestions.is_open()) oquestions.close();
    iquestions.open("../../../data/questions.json"); // pre production
    oquestions.open("../../../data/questions.json"); // pre production
    // if (!iquestions.is_open()) iquestions.open("questions.json");
    // if (!oquestions.is_open()) oquestions.open("questions.json");
}

void Data::save(){
    if (!oquestions.is_open()) open_io_gate();
    oquestions << dataset;
}

int Data::newSet(std::string_view setName){
    if (!nameValid(setName)) return 1;
    if (setExist(setName)) return 2;
    dataset[setName] = {};
    save();
    return 0;
}

int Data::addCard(std::string_view setName, Card card){
    using namespace std::chrono; 

    if (!nameValid(setName)) return 1;
    if (!setExist(setName)) return 3; 
    nlohmann::json& pData = dataset[setName];
    pData["front"] = card.front;
    pData["back"] = card.back;
    auto now = system_clock::now() + hours(7);   
    pData["lastRefresh"] = date::format("%F %T", now);
    save();
    return 0;
}

int Data::listSets(std::vector<std::string>& out){
    if (!out.empty()) out = {};
    for (auto it = dataset.begin(); it < dataset.end(); ++it){
        out.push_back(it.key());
    }
    save();
    return 0;
}

int Data::listCards(std::vector<nlohmann::json>& out, std::string_view setName){
    if (!out.empty()) out = {};
    if (!nameValid(setName)) return 1;
    if (!setExist(setName)) return 3; 
    for (auto it = dataset[setName].begin(); it < dataset[setName].end(); ++it){
        out.push_back(*it);
    }
    save();
    return 0;
}

bool Data::setExist(std::string_view setName){
    bool flag = false;
    for (auto it = dataset.begin(); it < dataset.end(); ++it){
        flag = (it.key() == setName);
    }
    return flag;
}

// return the index (1-based) at which the card has the same front. returns -1 if no duplication
int Data::cardExist(std::string_view setName, Card card){
    if (!setExist(setName)) return 0; 
    for (auto it = dataset[setName].begin(); it < dataset[setName].end(); ++it){
        if ((*it)["front"] == card.front) {
            return static_cast<int>(std::distance(dataset[setName].begin(), it)) + 1;
        }
    }
    return -1;
}

bool Data::nameValid(std::string_view name){
    static std::vector<std::string> DisallowedNames = {
        "help", "set", "quiz", "learn", "about", "new", "kill", "rename", "list", "add", "delete",
        "$set", "$item"
    };
    auto it = std::find(DisallowedNames.begin(), DisallowedNames.end(), name);

    return (it == DisallowedNames.end());
}

int Data::setSize(std::string_view setName){
    if (!setExist(setName)) return 0; 
    return dataset[setName].size();
}