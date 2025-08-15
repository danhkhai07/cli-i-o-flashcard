#include "cli/cli.h"
  
#include <csignal>
#include <iostream>

void handleSigint(int) {
    std::cout << "\nCtrl+C detected. Exiting...\n";
    std::cin.setstate(std::ios::failbit); // force all cin reads to fail
    std::exit(0);
}

int main(int agrc, char* agrv[]){
    // Handles Ctrl+C
    std::signal(SIGINT, handleSigint);

    App app;
    app.run(agrc, agrv);
    return 0;
}