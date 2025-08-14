#include "cli/cli.h"

#include <iostream>
#include <utility>

void App::run(int argc, char* argv[]){
    Command cmd;
    ExecutingOutput exeResult = cmd.lookUp(0, argc, argv, 0);
    help(argc, argv, exeResult);
    return;
}

/*
    Return value
    -1: internal error
    0: no error
    1: invalid argument format 
    2: missing argument
    3: unknown command
    4: conflicting options
    5: too many arguments
    6: invalid name
    7: name already exists (if try to create new)
    8: set doesn't exist (if try to access)
    9: too many item in set
    10: card index doesn't exist (in a set)
*/

void App::help(int argc, char* argv[], ExecutingOutput exeResult){
    std::cout << exeResult.errorCode << ' ' << exeResult.errorPos << '\n';
    return;
}