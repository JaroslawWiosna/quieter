#pragma once
#include "Task.hpp"
#include "Opts.hpp"

struct Table {
    Columns cols;
    Width width;
};

Width calc_width_of_desc(Table &table, const Opts &opts);
Table tasks_to_output_table(const Table&);
void print_table(Table table, const Tasks &tasks, const Opts &opts);
