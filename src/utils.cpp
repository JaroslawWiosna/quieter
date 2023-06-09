#include "utils.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <optional>
#include <map>
#include <filesystem>

// https://stackoverflow.com/a/23370070 for w.ws_row from struct winsize w;
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

/* Clear screen of terminal
 * https://codereview.stackexchange.com/a/198137
 */
void clear_screen() {
    std::cout << "\033c";
}

static int get_width_of_terminal() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // printf ("lines %d\n", w.ws_row);
    // printf ("columns %d\n", w.ws_col);
    return w.ws_col;
}

std::string castPolishLettersToASCII(const std::string& str) {
    std::string result = str;
    
    // Replace Polish letters with ASCII equivalents
    size_t pos;
    while ((pos = result.find("ą")) != std::string::npos) {
        result.replace(pos, 2, "a");
    }
    while ((pos = result.find("ć")) != std::string::npos) {
        result.replace(pos, 2, "c");
    }
    while ((pos = result.find("ę")) != std::string::npos) {
        result.replace(pos, 2, "e");
    }
    while ((pos = result.find("ł")) != std::string::npos) {
        result.replace(pos, 2, "l");
    }
    while ((pos = result.find("ń")) != std::string::npos) {
        result.replace(pos, 2, "n");
    }
    while ((pos = result.find("ó")) != std::string::npos) {
        result.replace(pos, 2, "o");
    }
    while ((pos = result.find("ś")) != std::string::npos) {
        result.replace(pos, 2, "s");
    }
    while ((pos = result.find("ź")) != std::string::npos) {
        result.replace(pos, 2, "z");
    }
    while ((pos = result.find("ż")) != std::string::npos) {
        result.replace(pos, 2, "z");
    }
    
    while ((pos = result.find("Ą")) != std::string::npos) {
        result.replace(pos, 2, "A");
    }
    while ((pos = result.find("Ć")) != std::string::npos) {
        result.replace(pos, 2, "C");
    }
    while ((pos = result.find("Ę")) != std::string::npos) {
        result.replace(pos, 2, "E");
    }
    while ((pos = result.find("Ł")) != std::string::npos) {
        result.replace(pos, 2, "L");
    }
    while ((pos = result.find("Ń")) != std::string::npos) {
        result.replace(pos, 2, "N");
    }
    while ((pos = result.find("Ó")) != std::string::npos) {
        result.replace(pos, 2, "O");
    }
    while ((pos = result.find("Ś")) != std::string::npos) {
        result.replace(pos, 2, "S");
    }
    while ((pos = result.find("Ź")) != std::string::npos) {
        result.replace(pos, 2, "Z");
    }
    while ((pos = result.find("Ż")) != std::string::npos) {
        result.replace(pos, 2, "Z");
    }
    return result;
}
