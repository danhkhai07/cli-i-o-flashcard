#include "cli/cli.h"
#include "commands/commands.h"

#include <iostream>

void App::run(int argc, const char* argv[]){
    Command cmd;
    if (cmd.lookUp(argc-1, argv+1, 1)){
        std::cout << "Invalid argument.";
        return;
    }
    return;
}