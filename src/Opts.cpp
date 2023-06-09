#include "Opts.hpp"

Args parse_argv(int argc, char **argv) {
    Args res{};
    for (int i{}; i<argc; ++i) {
        res.push_back(argv[i]);
    }
    // for (auto &item : res) {
    //     std::cout << item << '\n';
    // }
    return res;
}

struct Opts {
    std::string program{}; // binary name
    std::string path;
    std::optional<Width> table_width_set_via_argv{};
};

Opts args_to_opts(const Args &args) {
    Opts res{};
    res.program = args[0];

    if (args.size() < 1) {
        std::cerr 
            << "Usage: " 
            << res.program 
            << " [datebook.txt|directory_full_of_datebooks]"
            << std::endl
            << "ERROR: neither datebook nor directory is provided"
            << std::endl;
        exit(1);
    }

    for (size_t i{1}; i<args.size(); ++i) {
        const std::string prefix_width = "--width=";
        if (has_prefix(args[i], prefix_width)) {
            try {
                res.table_width_set_via_argv = std::stoi(args[i].substr(prefix_width.size()));
            } catch (std::invalid_argument const &e) {
                std::cerr 
                    << "Usage: " 
                    << res.program 
                    << " [datebook.txt|directory_full_of_datebooks] [--width=NUMBER]"
                    << std::endl
                    << "ERROR: cannot understand your NUMBER: " 
                    << args[i]
                    << std::endl;
                exit(1);
            }
        } else {
            res.path = args[i];
        }
    }

    if (res.program.empty()) {
        std::cerr 
            << "Usage: " 
            << res.program 
            << "[datebook.txt|directory_full_of_datebooks]"
            << std::endl
            << "ERROR: neither datebook nor directory is provided"
            << std::endl;
        exit(1);
    }

    return res;
}
