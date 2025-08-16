#pragma once

#include "dataHandler_export.h"
#include "data/card/card.h"

#include "utils/date/date.h"
#include "utils/nlohmann/json.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>

class DATAHANDLER_SHARED Data {
private:
     /**
     * @brief Get directory of program's executable.
     * @return Path to executable directory
     */
    std::filesystem::path getExecutableDirectory(char* argv0);

    /**
     * @brief Loads the dataset from questions.json.
     * @return 1 if loading succeeds, 0 if the file does not exist or parsing fails.
     */
    int load();

    /**
     * @brief Saves the dataset to questions.json.
     * @return 1 if saving succeeds, 0 if writing fails.
     */
    int save();

    nlohmann::json dataset;
    std::filesystem::path targetPath;
    
public:
    Data(){}
    Data(int argc, char* argv[]) {
        targetPath = getExecutableDirectory(argv[0]);
        load();
    }
    ~Data() {
        save();
    }

    // EXECUTING FUNCTIONS

    /**
     * @brief Creates a new set with the given name.
     * @param setName The name of the new set.
     * @return 0 on success, 6 if the name is invalid, 7 if the set already exists.
     */
    int newSet(std::string_view setName);

    /**
     * @brief Adds a card to the specified set.
     * @param setName The name of the set.
     * @param front The front text of the card.
     * @param back The back text of the card.
     * @return 0 on success, 8 if set doesn't exist, 9 if set is full.
     */
    int addCard(std::string_view setName, std::string_view front, std::string_view back);

    /**
     * @brief Deletes the specified set.
     * @param setName The name of the set to delete.
     * @return 0 on success, 8 if the set doesn't exist.
     */
    int killSet(std::string_view setName);

    /**
     * @brief Deletes a card at the given position in the set.
     * @param setName The name of the set.
     * @param cardPos The index of the card to delete.
     * @return 0 on success, 8 if set doesn't exist, 10 if card index is invalid.
     */
    int deleteCard(std::string_view setName, const int cardPos);

    /**
     * @brief Renames a set.
     * @param setName The current name of the set.
     * @param newName The new name for the set.
     * @return 0 on success, 8 if set doesn't exist, 6 if new name is invalid.
     */
    int renameSet(std::string_view setName, std::string_view newName);

    /**
     * @brief Clears the dataset and overwrites questions.json with an empty object.
     */
    void flushFile();

    // STATE-CHECKING FUNCTIONS

    /**
     * @brief Lists all set names in the dataset.
     * @param[out] out Reference to a vector that will be filled with set names.
     * @return 0 (always succeeds).
     */
    int listSets(std::vector<std::string>& out);

    /**
     * @brief Lists all cards in the specified set.
     * @param[out] out Reference to a vector that will be filled with card JSON objects.
     * @param setName The name of the set to list cards from.
     * @return 0 on success, 8 if set doesn't exist.
     */
    int listCards(std::vector<nlohmann::json>& out, std::string_view setName);

    /**
     * @brief Checks if a set exists.
     * @param setName The name of the set.
     * @return true if the set exists, false otherwise.
     */
    bool setExist(std::string_view setName);

    /**
     * @brief Returns the index (1-based) of a card with the same front in the set.
     * @param setName The name of the set.
     * @param front The front text to search for.
     * @return Index (1-based) if found, -1 if not found, 0 if set doesn't exist.
     */
    int cardContentExist(std::string_view setName, std::string_view front);

    /**
     * @brief Checks if a card index exists in the set.
     * @param setName The name of the set.
     * @param idx The index to check.
     * @return true if valid, false otherwise.
     */
    bool cardIdxExist(std::string_view setName, const int idx);

    /**
     * @brief Checks if a name is valid (no spaces, not disallowed).
     * @param name The name to check.
     * @return true if valid, false otherwise.
     */
    bool nameValid(std::string_view name);

    /**
     * @brief Returns the number of cards in the set.
     * @param setName The name of the set.
     * @return Number of cards in the set, or 0 if set doesn't exist.
     */
    int setSize(std::string_view setName);

    /**
     * @brief Returns if the set is full.
     * @param setName The name of the set.
     * @return Returns true if set is full (> 2*1e9 items), false if not or set doesn't exist.
     */
    bool setFull(std::string_view setName);
};