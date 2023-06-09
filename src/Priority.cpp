#include "Priority.hpp"

std::string to_string(Priority p) {
    switch(p) {
        case Priority::MUST:
            return "Must";
        case Priority::SHOULD:
            return "Should";
        case Priority::COULD:
            return "Could";
        case Priority::WONT:
            return "Won't";
        default:
            throw;
            return "???";
    }
}
