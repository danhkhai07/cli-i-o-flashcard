#include "cli/cli.h"

#include <iostream>

void App::run(int argc, const char* argv[]){
    if (argc < 2) {
        functionMap["help"](argc,argv); 
        return;
    }

    auto it = functionMap.find(argv[1]);
    if (it != functionMap.end()){
        it->second(argc,argv);
    } else {
        std::cout << "Command not found.\n";
        std::cout << "Use '.\\quiz.exe help' to read about available commands.";
    }

    return;
}