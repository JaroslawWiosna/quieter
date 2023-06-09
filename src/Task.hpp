#pragma once

#include <string>
#include <vector>
#include <optional>
#include <map>

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
