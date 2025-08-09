#pragma once

#include <unordered_map>
#include <string>
#include <functional>


class App{
    public:
        // Default constructor and destructor
        App(){};
        ~App(){};

        void run(int agrc, char* agrv[]);
        void help(int argc, char* argv[], std::pair<int, int> errorOut);

    private:
        // Declaring (but not defining) to disallow copy constructor and operator=
        App(const App&);
        App& operator=(const App&);

};