#pragma once
#include <vector>
#include <string>

using Args = std::vector<std::string>;
Args parse_argv(int argc, char **argv);

class Opts;
Opts args_to_opts(const Args &args);
