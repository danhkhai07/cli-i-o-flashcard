#include "data/dataHandler/dataHandler.h"

#include <vector>
#include <algorithm>
#include <fstream>
#include <climits>
#include <chrono>

static const int MAXIMUM_SET_SIZE = 2*1e9;

std::filesystem::path Data::getExecutableDirectory(char* argv0){
    std::filesystem::path pth = std::filesystem::canonical(std::filesystem::path(argv0)).parent_path();
    pth = pth / "../data/questions.json";
    return std::filesystem::weakly_canonical(pth);
}

int Data::load(){
    std::ifstream in(targetPath);
    if (!in) {
        std::cerr << "Error: could not open file for reading: " << targetPath << "\n";
        dataset = nlohmann::json::object();
        return 0;
    }

    try {
        dataset = nlohmann::json::parse(in);
    } catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        dataset = nlohmann::json::object();
        return 0;
    }

    return 1;
}

int Data::save(){
    std::ofstream out(targetPath);
    if (!out) {
        std::cerr << "Error: could not open file for writing: " << targetPath << "\n";
        return 0;
    }

    out << dataset.dump(4);
    if (!out) {
        std::cerr << "Error: write failed\n";
        return 0;
    }

    return 1;
}

int Data::newSet(std::string_view setName){
    if (!nameValid(setName)) return 6;   // invalid name
    if (setExist(setName)) return 7;     // set already exists
    dataset[setName] = {};
    save();
    return 0;
}

int Data::addCard(std::string_view setName, std::string_view front, std::string_view back){
    using namespace std::chrono;

    if (!setExist(setName)) return 8;        // set does not exist
    if (setSize(setName) >= MAXIMUM_SET_SIZE) return 9; // set full

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
    if (!setExist(setName)) return 8; // set does not exist
    dataset.erase(setName);
    save();
    return 0;
}

int Data::deleteCard(std::string_view setName, const int cardPos){
    if (!setExist(setName)) return 8;   // set does not exist
    if (!cardIdxExist(setName, cardPos)) return 10; // invalid card index

    dataset[setName].erase(dataset[setName].begin() + cardPos);
    save();
    return 0;
}

int Data::renameSet(std::string_view setName, std::string_view newName){
    if (!setExist(setName)) return 8; // set does not exist
    if (!nameValid(newName)) return 6; // invalid new name

    dataset[newName] = dataset[setName];
    dataset.erase(setName);
    save();
    return 0;
}

void Data::flushFile(){
    dataset = nlohmann::json::object();
    save();
}

int Data::listSets(std::vector<std::string>& out){
    if (!out.empty()) out.clear();
    for (auto it = dataset.begin(); it != dataset.end(); ++it){
        out.push_back(it.key());
    }
    return 0;
}

int Data::listCards(std::vector<nlohmann::json>& out, std::string_view setName){
    if (!out.empty()) out.clear();
    if (!setExist(setName)) return 8; // set does not exist
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

bool Data::cardIdxExist(std::string_view setName, const int idx){
    if (!setExist(setName)) return false;
    if (idx < 0 || idx >= setSize(setName)) return false;
    return true;
}

bool Data::nameValid(std::string_view name){
    bool containSpace = (name.find(' ') != std::string_view::npos);

    static const std::vector<std::string> DisallowedNames = {
        "help", "set", "quiz", "learn", "about", "new", "kill", "rename", "list", "add", "delete",
        "$set", "$item", "$newSetName", "--set", "-s", "-i", "--item"
    };
    bool disallowed = (std::find(DisallowedNames.begin(), DisallowedNames.end(), name) != DisallowedNames.end());

    return !(containSpace || disallowed);
}

int Data::setSize(std::string_view setName){
    if (!setExist(setName)) return 0;
    return static_cast<int>(dataset[setName].size());
}
