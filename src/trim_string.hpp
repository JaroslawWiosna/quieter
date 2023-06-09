#pragma once

#include <string>

// trim from start (in place)
static inline void ltrim(std::string &);

// trim from end (in place)
static inline void rtrim(std::string &);

// trim from both ends (in place)
static inline void trim(std::string &);

// trim from start (copying)
static inline std::string ltrim_copy(std::string);

// trim from end (copying)
static inline std::string rtrim_copy(std::string);

// trim from both ends (copying)
static inline std::string trim_copy(std::string);

bool has_prefix(std::string s, std::string prefix);