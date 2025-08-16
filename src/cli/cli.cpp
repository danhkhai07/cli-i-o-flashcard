#include "cli/cli.h"

#include <iostream>
#include <utility>

void App::run(int argc, char* argv[]){
    Command cmd(argc,argv);
    ExecutingOutput exeOut = cmd.lookUp(0, argc, argv, 0, {});
    cmd.resolveExecutingOutput(argc, argv, exeOut);
    return;
}