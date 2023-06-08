#pragma once

#include <string>
#include <vector>
#include <optional>
#include <map>

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
float urgency_coefficient_prio(Priority p);
float urgency_due(int days);

////////////////////////////////////////////////////////////////////////

using Day = int;
using Id = int;
using Ids = std::vector<Id>;

struct Task {
    std::vector<std::string> lines;
    bool has_dl;
    Day dl; // deadline
    Priority prio;
    std::string desc;
    Id id{};
    std::optional<Id> parent;
    Ids children;
    int indent;
    fs::path file;
};

using Tasks = std::vector<Task>;

float urgency(Task task);

// https://stackoverflow.com/a/22054031
bool has_prefix(std::string s, std::string prefix);

Tasks tasks;

int diff_in_days(std::string s);

bool asc_pred(Task a, Task b);
Priority scrap_prio(std::string &s);

const std::string keyword_due{"due:"};
std::optional<Day> scrap_due(std::string &s);
//print w/ padding
void pad_print_left(std::string s, long unsigned int pad, char chr);
void pad_print_right(std::string s, long unsigned int pad, char chr);
void pad_print(std::string s, long unsigned int pad, char chr);
std::optional<Id> get_most_recent_id_of_given_indent(int indent);
int id{}; // od zera bo jestesmy programystami
void parse_file_to_tasks(fs::path filepath);
void parse_dir_to_tasks(fs::path dirpath);

enum class Title {
    ID,
    PRIO,
    PARENT,
    DEPS,
    DL,
    DESC,   
    FILENAME,
    URG,
};

std::string to_string(Title title);
using Width = long unsigned int;
using Columns = std::map<Title, Width>;

struct Table {
    Columns cols;
    Width width;
};

Width calc_width_of_desc(Table &table, const Opts &opts);
Table tasks_to_output_table();
void print_table(Table table, const Opts &opts);
