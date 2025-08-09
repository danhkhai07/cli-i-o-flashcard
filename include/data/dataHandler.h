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

        int load();
        int save();

    public:
        Data(){
            load();
        }
        ~Data() {
            save();
        }
    
    // EXECUTING FUNCTIONS
        int newSet(std::string_view setName);
        int addCard(std::string_view setName, std::string_view front, std::string_view back);

        int killSet(std::string_view setName);
        int deleteCard(std::string_view setName, const int cardPos);

        int renameSet(std::string_view setName, std::string_view newName);

        void flushFile();

    // STATE-CHECKING FUNCTIONS
        int listSets(std::vector<std::string>& out);
        int listCards(std::vector<nlohmann::json>& out, std::string_view setName);
        bool setExist(std::string_view setName);
        int cardContentExist(std::string_view setName, std::string_view front);
        int cardIdxExist(std::string_view setName, const int idx);
        bool nameValid(std::string_view name);
        int setSize(std::string_view setName);

};