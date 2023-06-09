#include "Table.hpp"
#include "Opts.hpp"

#include <iostream>

Width calc_width_of_desc(Table &table, const Opts &opts) {
    Width max = opts.table_width_set_via_argv.value_or(get_width_of_terminal());
    Width others = table.cols[Title::ID] + 1 + table.cols[Title::PRIO] + 1 + table.cols[Title::PARENT] + 1 + table.cols[Title::DEPS] + 1 + table.cols[Title::DL] + 1 + table.cols[Title::FILENAME] + 1 + table.cols[Title::URG] + 4;

    if (others >= max) {
        std::cerr 
            << "Usage: " 
            << opts.program 
            << " [datebook.txt|directory_full_of_datebooks] [--width=NUMBER]"
            << std::endl
            << "ERROR: your NUMBER("
            << opts.table_width_set_via_argv.value()
            << ") is too small, so I cannot render table" 
            << std::endl;
        exit(1);
    }
    return max - others;
}

Table tasks_to_output_table() {
    Table res{};
    res.cols[Title::ID] = to_string(Title::ID).size();
    res.cols[Title::PRIO] = to_string(Title::PRIO).size();
    res.cols[Title::PARENT] = to_string(Title::PARENT).size();
    res.cols[Title::DEPS] = to_string(Title::DEPS).size();
    res.cols[Title::DL] = to_string(Title::DL).size();
    res.cols[Title::DESC] = to_string(Title::DESC).size();
    res.cols[Title::FILENAME] = to_string(Title::FILENAME).size();
    res.cols[Title::URG] = to_string(Title::URG).size();

    for (const auto &task : tasks) {
        {

            long unsigned int width = std::to_string(task.id).size();
            if (width > res.cols[Title::ID]) {
                res.cols[Title::ID] = width;
            }
        }

        if (Priority::NOT_SPECIFIED != task.prio) {
            long unsigned int width = to_string(task.prio).size();
            if (width > res.cols[Title::PRIO]) {
                res.cols[Title::PRIO] = width;
            }
        }
        if (task.parent.has_value()) {
            long unsigned int width = std::to_string(task.parent.value()).size();
            if (width > res.cols[Title::PARENT]) {
                res.cols[Title::PARENT] = width;
            }
        }
        if (task.children.size() > 0) {
            std::string deps{}; // task depends on its children, aka subtasks
            for (const auto &child : task.children) {
                deps += std::to_string(child);
                deps += ", ";
            }
            long unsigned int width = deps.size();
            if (width > res.cols[Title::DEPS]) {
                res.cols[Title::DEPS] = width;
            }
        }
        if (task.has_dl) {
            long unsigned int width = std::to_string(task.dl).size();
            if (width > res.cols[Title::DL]) {
                res.cols[Title::DL] = width;
            }
        }
        {
            long unsigned int width = task.desc.size();
            if (width > res.cols[Title::DESC]) {
                res.cols[Title::DESC] = width;
            }
        }
        {            
            long unsigned int width = std::string{task.file.stem()}.size();
            if (width > res.cols[Title::FILENAME]) {
                res.cols[Title::FILENAME] = width;
            }
        }
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1) << urgency(task);
            long unsigned int width = stream.str().size();
            if (width > res.cols[Title::URG]) {
                res.cols[Title::URG] = width;
            }
        }
    }
    return res;
}   

void print_table(Table table, const Opts &opts) {

    // table.cols[Title::PARENT] = 3;

    table.cols[Title::DESC] = calc_width_of_desc(table, opts);
    
    pad_print(to_string(Title::ID), table.cols[Title::ID]);
    std::cout << ' ';
    pad_print(to_string(Title::PRIO), table.cols[Title::PRIO]);
    std::cout << ' ';
    pad_print(to_string(Title::PARENT), table.cols[Title::PARENT]);
    std::cout << ' ';
    pad_print(to_string(Title::DEPS), table.cols[Title::DEPS]);
    std::cout << ' ';
    pad_print(to_string(Title::DL), table.cols[Title::DL]);
    std::cout << ' ';
    pad_print_right(to_string(Title::DESC), table.cols[Title::DESC]);
    std::cout << ' ';
    pad_print(to_string(Title::FILENAME), table.cols[Title::FILENAME]);    
    std::cout << ' ';
    pad_print(to_string(Title::URG), table.cols[Title::URG]);    
    std::cout << '\n';


    for (const auto &task : tasks) {
        pad_print(trim_copy(std::to_string(task.id)), table.cols[Title::ID]);
        std::cout << ' ';

        if (task.prio != Priority::NOT_SPECIFIED) {
            pad_print(std::string{to_string(task.prio)}, table.cols[Title::PRIO]);
        } else {
            pad_print({}, table.cols[Title::PRIO]);
        }
        if (task.parent.has_value()) {
            pad_print(std::to_string(task.parent.value()), table.cols[Title::PARENT]);
        } else {
            pad_print({}, table.cols[Title::PARENT]);
        }
        std::cout << ' ';
        {
            std::string deps{}; // task depends on its children, aka subtasks
            for (const auto &child : task.children) {
                deps += std::to_string(child);
                deps += ", ";
            }
            if (deps.size() > 0) {
                // remove last `, `
                deps.pop_back(); 
                deps.pop_back();
            }
            pad_print(deps, table.cols[Title::DEPS]);
        }
        std::cout << ' ';
        if (task.has_dl) {
            pad_print(std::to_string(task.dl), table.cols[Title::DL]);
        } else {
            pad_print({}, table.cols[Title::DL]);
        }
        std::cout << ' ';
        pad_print_right(trim_copy(task.desc), table.cols[Title::DESC]);
        std::cout << ' ';
        pad_print(trim_copy(task.file.stem()), table.cols[Title::FILENAME]);
        std::cout << ' ';
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1) << urgency(task);
            pad_print(stream.str(), table.cols[Title::URG]);
        }
        std::cout << std::endl;
    }
}
