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
    
        int newSet(std::string_view setName);
        int addCard(std::string_view setName, std::string_view front, std::string_view back);
        int listSets(std::vector<std::string>& out);
        int listCards(std::vector<nlohmann::json>& out, std::string_view setName);

        bool setExist(std::string_view setName);
        int cardExist(std::string_view setName, std::string_view front);
        bool nameValid(std::string_view name);
        int setSize(std::string_view setName);

};