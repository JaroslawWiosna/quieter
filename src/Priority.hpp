#pragma once

enum class Priority {
    NOT_SPECIFIED = 0,
    MUST,
    SHOULD,
    COULD,
    WONT,
    COUNT
};
static_assert(static_cast<int>(Priority::COUNT) == 5);

std::string to_string(Priority p);