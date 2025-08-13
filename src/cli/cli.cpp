#include "cli/cli.h"
#include "commands/commands.h"
#include "data/dataHandler/dataHandler.h"

#include <iostream>
#include <utility>

void App::run(int argc, char* argv[]){
    Command cmd;
    // cmd.errorHandling(argc, argv, cmd.lookUp(1, argc, argv, 0).first);
    std::pair<int, int> exeResult = cmd.lookUp(0, argc, argv, 0);
    std::cout << " " << exeResult.first << ' ' << exeResult.second << "\n";
    /*
        a value
            -1: internal error
            0: no error
            1: invalid argument format 
            2: missing argument
            3: unknown command
            4: conflicting options
            5: too many arguments
    */
    return;
}