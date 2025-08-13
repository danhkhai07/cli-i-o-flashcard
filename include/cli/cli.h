#pragma once

#include "cli_export.h"

#include <unordered_map>
#include <string>
#include <functional>


class CLI_SHARED App{
    public:
        // Default constructor and destructor
        App(){};
        ~App(){};

        void run(int agrc, char* agrv[]);
        void help(int argc, char* argv[], std::pair<int, int> exeResult);

    private:
        // Declaring (but not defining) to disallow copy constructor and operator=
        App(const App&);
        App& operator=(const App&);

};