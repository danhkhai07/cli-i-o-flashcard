#pragma once

class Command {
    public:
        void lookUp(int argc, const char* argv[], const int& level);
    private:
        void help(int argc, const char* argv[]);
        void learn(int argc, const char* argv[]);
        
        void item(int argc, const char* argv[]);
        void itemAdd(int argc, const char* argv[]);
        void itemList(int argc, const char* argv[]);
};