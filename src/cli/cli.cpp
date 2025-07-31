#include "cli/cli.h"
#include "commands/commands.h"

#include <iostream>

void App::run(int argc, const char* argv[]){
    Command cmd;
    cmd.lookUp(argc, argv, 1);
    return;
}