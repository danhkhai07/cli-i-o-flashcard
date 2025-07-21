#pragma once

namespace Command {
    void help(int agrc, const char* agrv[]);
    void learn(int agrc, const char* agrv[]);
    void addItem(int agrc, const char* agrv[]);
    void showItemList(int agrc, const char* agrv[]);
}