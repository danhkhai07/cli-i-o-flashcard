#include "data/dataHandler.h"

#include <vector>
#include <algorithm>

/**
 * @brief Loads the dataset from questions.json.
 * @return 1 if loading succeeds, 0 if the file does not exist or parsing fails.
 */
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

/**
 * @brief Saves the dataset to questions.json.
 * @return 1 if saving succeeds, 0 if writing fails.
 */
int Data::save(){
    std::ofstream out("questions.json", std::ios::out | std::ios::trunc);
    out << dataset.dump(4); // pretty print
    if (!out) return 0;
    return 1;
}

/**
 * @brief Creates a new set with the given name.
 * @param setName The name of the new set.
 * @return 0 on success, 1 if the name is invalid, 2 if the set already exists.
 */
int Data::newSet(std::string_view setName){
    if (!nameValid(setName)) return 1;
    if (setExist(setName)) return 2;
    dataset[setName] = {};
    save();
    return 0;
}

/**
 * @brief Adds a card to the specified set.
 * @param setName The name of the set.
 * @param front The front text of the card.
 * @param back The back text of the card.
 * @return 0 on success, 1 if set name is invalid, 3 if set doesn't exist, 4 if set is full.
 */
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

/**
 * @brief Deletes the specified set.
 * @param setName The name of the set to delete.
 * @return 0 on success, 3 if the set doesn't exist.
 */
int Data::killSet(std::string_view setName){
    if (!setExist(setName)) return 3;

    dataset.erase(setName);
    save();
    return 0;
}

/**
 * @brief Deletes a card at the given position in the set.
 * @param setName The name of the set.
 * @param cardPos The index of the card to delete.
 * @return 0 on success, 3 if set doesn't exist, 5 if card index is invalid.
 */
int Data::deleteCard(std::string_view setName, const int cardPos){
    if (!setExist(setName)) return 3;
    if (!cardIdxExist(setName, cardPos)) return 5;
    
    dataset[setName].erase(dataset[setName].begin() + cardPos);
    save();
    return 0;
}

/**
 * @brief Renames a set.
 * @param setName The current name of the set.
 * @param newName The new name for the set.
 * @return 0 on success, 3 if set doesn't exist, 1 if new name is invalid.
 */
int Data::renameSet(std::string_view setName, std::string_view newName){
    if (!setExist(setName)) return 3;
    if (!nameValid(newName)) return 1;

    dataset[newName] = dataset[setName];
    dataset.erase(setName);
    save();
    return 0;
}

/**
 * @brief Clears the dataset and overwrites questions.json with an empty object.
 */
void Data::flushFile(){
    dataset = nlohmann::json::object();
    save();
    return;
}

/**
 * @brief Lists all set names in the dataset.
 * @param[out] out Reference to a vector that will be filled with set names.
 * @return 0 (always succeeds).
 */
int Data::listSets(std::vector<std::string>& out){
    if (!out.empty()) out = {};
    for (auto it = dataset.begin(); it != dataset.end(); ++it){
        out.push_back(it.key());
    }
    return 0;
}

/**
 * @brief Lists all cards in the specified set.
 * @param[out] out Reference to a vector that will be filled with card JSON objects.
 * @param setName The name of the set to list cards from.
 * @return 0 on success, 3 if set doesn't exist.
 */
int Data::listCards(std::vector<nlohmann::json>& out, std::string_view setName){
    if (!out.empty()) out = {};
    if (!setExist(setName)) return 3; 
    for (auto it = dataset[setName].begin(); it != dataset[setName].end(); ++it){
        out.push_back(*it);
    }
    return 0;
}

/**
 * @brief Checks if a set exists.
 * @param setName The name of the set.
 * @return true if the set exists, false otherwise.
 */
bool Data::setExist(std::string_view setName){
    for (auto it = dataset.begin(); it != dataset.end(); ++it){
        if (it.key() == setName) return true;
    }
    return false;
}

/**
 * @brief Returns the index (1-based) of a card with the same front in the set.
 * @param setName The name of the set.
 * @param front The front text to search for.
 * @return Index (1-based) if found, -1 if not found, 0 if set doesn't exist.
 */
int Data::cardContentExist(std::string_view setName, std::string_view front){
    if (!setExist(setName)) return 0; 
    for (auto it = dataset[setName].begin(); it != dataset[setName].end(); ++it){
        if ((*it)["front"] == front) {
            return static_cast<int>(std::distance(dataset[setName].begin(), it)) + 1;
        }
    }
    return -1;
}

/**
 * @brief Checks if a card index exists in the set.
 * @param setName The name of the set.
 * @param idx The index to check.
 * @return 1 if valid, 0 otherwise.
 */
int Data::cardIdxExist(std::string_view setName, const int idx){
    if (!setExist(setName)) return 0;
    if (idx < 0 or idx >= setSize(setName)) return 0;
    return 1;
}

/**
 * @brief Checks if a name is valid (no spaces, not disallowed).
 * @param name The name to check.
 * @return true if valid, false otherwise.
 */
bool Data::nameValid(std::string_view name){
    bool containSpace = (name.find(' ') != std::string_view::npos);
    
    static std::vector<std::string> DisallowedNames = {
        "help", "set", "quiz", "learn", "about", "new", "kill", "rename", "list", "add", "delete",
        "$set", "$item"
    };
    bool disallowed = (std::find(DisallowedNames.begin(), DisallowedNames.end(), name) != DisallowedNames.end());

    return !(containSpace or disallowed);
}

/**
 * @brief Returns the number of cards in the set.
 * @param setName The name of the set.
 * @return Number of cards in the set, or 0 if set doesn't exist.
 */
int Data::setSize(std::string_view setName){
    if (!setExist(setName)) return 0; 
    return dataset[setName].size();
}