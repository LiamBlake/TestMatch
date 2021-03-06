#include "testmatch/enums.hpp"

#include <stdexcept>
#include <string>

std::string str(Arm arm) {
    switch (arm) {
        case left:
            return "left";
        case right:
            return "right";
        default:
            // Throw exception
            throw(std::invalid_argument("Undefined Arm value."));
    }
}

char chr(Arm arm) {
    switch (arm) {
        case left:
            return 'l';
        case right:
            return 'r';
        default:
            // Throw exception
            throw(std::invalid_argument("Undefined Arm value."));
    }
}

std::string str(BowlType bowltype) {
    switch (bowltype) {
        case med:
            return "m";
        case med_fast:
            return "mf";
        case fast_med:
            return "fm";
        case fast:
            return "f";
        case offbreak:
            return "ob";
        case legbreak:
            return "lb";
        default:
            // Throw exception
            throw(std::invalid_argument("Undefined BowlType value."));
    }
}

std::string long_str(Arm arm, BowlType bowltype) {
    std::string str_arm = str(arm) + "-arm";

    std::string str_bt;
    switch (bowltype) {
        case med:
            str_bt = "medium";
            break;
        case med_fast:
            str_bt = "medium-fast";
            break;
        case fast_med:
            str_bt = "fast-medium";
            break;
        case fast:
            str_bt = "fast";
            break;
        case offbreak:
            if (arm == left)
                str_bt = "wrist spin";
            else
                str_bt = "offspin";
            break;
        case legbreak:
            if (arm == left)
                str_bt = "orthodox";
            else
                str_bt = "wrist spin";
            break;
        default:
            // Throw exception
            throw(std::invalid_argument("Undefined BowlType value."));
    }

    return str_arm + " " + str_bt;
}

std::string str(DismType dismtype) {
    switch (dismtype) {
        case bowled:
            return "b";
        case lbw:
            return "lbw";
        case caught:
            return "c";
        case c_and_b:
            return "c&b";
        case run_out:
            return "ro";
        case stumped:
            return "st";
        default:
            // Throw exception
            throw(std::invalid_argument("Undefined DismType value."));
    }
}

std::string str(TossChoice tosschoice) {
    switch (tosschoice) {
        case bat:
            return "bat";
        case field:
            return "field";
        default:
            // Throw exception
            throw(std::invalid_argument("Undefined DismType value."));
    }
}