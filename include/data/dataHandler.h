#pragma once

#include "data/card.h"
#include "utils/date/date.h"
#include "utils/nlohmann/json.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <memory>

/*
    Return code (int):
    0: no error
    1: invalid name
    2: name already exists (if try to create new)
    3: set doesn't exist (if try to access)
    4: too many item in set
    5: card index doesn't exist (in a set)
*/

class Data {
private:
    nlohmann::json dataset;

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

public:
    Data() {
        load();
    }
    ~Data() {
        save();
    }

    // EXECUTING FUNCTIONS

    /**
     * @brief Creates a new set with the given name.
     * @param setName The name of the new set.
     * @return 0 on success, 1 if the name is invalid, 2 if the set already exists.
     */
    int newSet(std::string_view setName);

    /**
     * @brief Adds a card to the specified set.
     * @param setName The name of the set.
     * @param front The front text of the card.
     * @param back The back text of the card.
     * @return 0 on success, 1 if set name is invalid, 3 if set doesn't exist, 4 if set is full.
     */
    int addCard(std::string_view setName, std::string_view front, std::string_view back);

    /**
     * @brief Deletes the specified set.
     * @param setName The name of the set to delete.
     * @return 0 on success, 3 if the set doesn't exist.
     */
    int killSet(std::string_view setName);

    /**
     * @brief Deletes a card at the given position in the set.
     * @param setName The name of the set.
     * @param cardPos The index of the card to delete.
     * @return 0 on success, 3 if set doesn't exist, 5 if card index is invalid.
     */
    int deleteCard(std::string_view setName, const int cardPos);

    /**
     * @brief Renames a set.
     * @param setName The current name of the set.
     * @param newName The new name for the set.
     * @return 0 on success, 3 if set doesn't exist, 1 if new name is invalid.
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
     * @return 0 on success, 3 if set doesn't exist.
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
     * @return 1 if valid, 0 otherwise.
     */
    int cardIdxExist(std::string_view setName, const int idx);

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
};
