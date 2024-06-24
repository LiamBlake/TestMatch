
#include "testmatch/helpers.hpp"

#include "testmatch/enums.hpp"

#include <sstream>
#include <string>

bool is_slow_bowler(BowlType bowl_type) {
    switch (bowl_type) {
        case med:
            return true;
        case offbreak:
            return true;
        case legbreak:
            return true;
        default:
            return false;
    }
}

std::string print_rounded(double value, int precision) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}