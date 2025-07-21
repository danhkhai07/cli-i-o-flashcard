#pragma once

#include "commands/commands.h"

#include <unordered_map>
#include <string>
#include <functional>


class App{
    public:
        // Default constructor and destructor
        App(){
            functionMap["help"] = Command::help;
            functionMap["learn"] = Command::learn;
            functionMap["item"] = Command::item;
        };
        ~App(){};

        void run(int agrc, const char* agrv[]);

    private:
        // Declaring (but not defining) to disallow copy constructor and operator=
        App(const App&);
        App& operator=(const App&);

        std::unordered_map<std::string,std::function<void(int, const char*[])>> functionMap;
};