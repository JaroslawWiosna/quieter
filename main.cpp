#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <optional>

// https://stackoverflow.com/a/23370070 for w.ws_row from struct winsize w;
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

static int get_width_of_terminal() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // printf ("lines %d\n", w.ws_row);
    // printf ("columns %d\n", w.ws_col);
    return w.ws_col;
}

std::string castPolishLettersToASCII(const std::string& str) {
    std::string result = str;
    
    // Replace Polish letters with ASCII equivalents
    size_t pos;
    while ((pos = result.find("ą")) != std::string::npos) {
        result.replace(pos, 2, "a");
    }
    while ((pos = result.find("ć")) != std::string::npos) {
        result.replace(pos, 2, "c");
    }
    while ((pos = result.find("ę")) != std::string::npos) {
        result.replace(pos, 2, "e");
    }
    while ((pos = result.find("ł")) != std::string::npos) {
        result.replace(pos, 2, "l");
    }
    while ((pos = result.find("ń")) != std::string::npos) {
        result.replace(pos, 2, "n");
    }
    while ((pos = result.find("ó")) != std::string::npos) {
        result.replace(pos, 2, "o");
    }
    while ((pos = result.find("ś")) != std::string::npos) {
        result.replace(pos, 2, "s");
    }
    while ((pos = result.find("ź")) != std::string::npos) {
        result.replace(pos, 2, "z");
    }
    while ((pos = result.find("ż")) != std::string::npos) {
        result.replace(pos, 2, "z");
    }
    
    while ((pos = result.find("Ą")) != std::string::npos) {
        result.replace(pos, 2, "A");
    }
    while ((pos = result.find("Ć")) != std::string::npos) {
        result.replace(pos, 2, "C");
    }
    while ((pos = result.find("Ę")) != std::string::npos) {
        result.replace(pos, 2, "E");
    }
    while ((pos = result.find("Ł")) != std::string::npos) {
        result.replace(pos, 2, "L");
    }
    while ((pos = result.find("Ń")) != std::string::npos) {
        result.replace(pos, 2, "N");
    }
    while ((pos = result.find("Ó")) != std::string::npos) {
        result.replace(pos, 2, "O");
    }
    while ((pos = result.find("Ś")) != std::string::npos) {
        result.replace(pos, 2, "S");
    }
    while ((pos = result.find("Ź")) != std::string::npos) {
        result.replace(pos, 2, "Z");
    }
    while ((pos = result.find("Ż")) != std::string::npos) {
        result.replace(pos, 2, "Z");
    }
    return result;
}


// https://stackoverflow.com/a/217605
#include <algorithm> 
#include <cctype>
#include <locale>

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}


enum class Priority {
    NOT_SPECIFIED = 0,
    MUST,
    SHOULD,
    COULD,
    WONT,
    COUNT
};
static_assert(static_cast<int>(Priority::COUNT) == 5);

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

float urgency_coefficient_prio(Priority p) {
    switch(p) {
        case Priority::MUST:
            return 6.0f;
        case Priority::SHOULD:
            return 3.9f;
        case Priority::COULD:
            return 1.8f;
        case Priority::WONT:
            return 0.1f;
        default:
            throw;
    } 
}

float urgency_due(int days) {
//    Datetime due (get_date ("due"));
//
//    // Map a range of 21 days to the value 0.2 - 1.0
//    float days_overdue = (now - due) / 86400.0;
//    if (days_overdue >= 7.0)   return 1.0;   // < 1 wk ago
//    else if (days_overdue >= -14.0) return ((days_overdue + 14.0) * 0.8 / 21.0) + 0.2;
//    else                            return 0.2;   // > 2 wks
//
    if (days <= -7) {
        return 1.0f;
    } else if (days >= 14) {
        return 0.2f;
    } else {
        return (-8.f / 210.f) * days + (22.f / 30.f);
    }
}

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
};

using Tasks = std::vector<Task>;

float urgency(Task task) {
    float result{};
    if (task.has_dl) {
        result += urgency_due(task.dl);
    }
    if (Priority::NOT_SPECIFIED != task.prio) {
        result += urgency_coefficient_prio(task.prio);
    }
    if (task.parent.has_value()) {
        // you have parent, so you are blockin your parent.
        result += 8.f;
    }
    if (task.children.size() > 0) {
        // you have children aka subtasks, which are more urgent
        result -= 5.f;
    }
    return result;
}

// https://stackoverflow.com/a/22054031
bool has_prefix(std::string s, std::string prefix) {
    return (s.compare(0, prefix.size(), prefix) == 0);
}

Tasks tasks;

// TODO(#4): we need unittest for diff_in_days
// ...and some static_asertion
int diff_in_days(std::string s) {
    time_t t = time(NULL);
    // struct tm tm = *localtime(&t);

    struct tm other = *localtime(&t);

    int year = std::stoi(s.substr(0,4));
    int mon = std::stoi(s.substr(5,2));
    int day = std::stoi(s.substr(8,2));
    
    // struct tm now = *localtime(&t);
    // std::cout << year << '-' << mon << '-' << day << '\t' << now.tm_year <<  '-' << now.tm_mon << '-' << now.tm_mday << '\n';
            
    other.tm_year = year - 1900;
    other.tm_mon = mon - 1;
    other.tm_mday = day;
    
    double seconds = difftime(mktime(&other), t);
    return (seconds/(60.f * 60.f * 24));
}

bool asc_pred(Task a, Task b) {
    return urgency(a) > urgency(b);
}

Priority scrap_prio(std::string &s) {
    if (std::string::npos != s.find("prio:M")) {
        s.erase(s.find("prio:M"), std::string{"prio:M"}.size());
        return Priority::MUST;
    }
    if (std::string::npos != s.find("prio:S")) {
        s.erase(s.find("prio:S"), std::string{"prio:S"}.size());
        return Priority::SHOULD;
    }
    if (std::string::npos != s.find("prio:C")) {
        s.erase(s.find("prio:C"), std::string{"prio:C"}.size());
        return Priority::COULD;
    }
    if (std::string::npos != s.find("prio:W")) {
        s.erase(s.find("prio:W"), std::string{"prio:W"}.size());
        return Priority::WONT;
    }
    return Priority::NOT_SPECIFIED;
}

const std::string keyword_due{"due:"};
std::optional<Day> scrap_due(std::string &s) {
    const auto pos = s.find(keyword_due);
    if (std::string::npos != pos) {
        int result = diff_in_days(s.substr(s.find(keyword_due)+keyword_due.size(), std::string{"2000-00-00"}.size()));
        s = s.erase(pos, keyword_due.size()+std::string{"2000-00-00"}.size());
        return result;
    }
    return {}; 
}

// TODO: Don't allow multiple occurences of `[` and `]`
// This should be reserved only for `[ ]` and `[X]`

//print w/ padding
void pad_print_left(std::string s, long unsigned int pad, char chr = ' ') {
    std::string ns = s; // new string
    if (pad > ns.size()) {
        ns.insert(0, pad - ns.size(), chr);
    }
    std::cout << ns;
}

void pad_print_right(std::string s, long unsigned int pad, char chr = ' ') {
    std::string ns = s; // new string
    if (pad > ns.size()) {
        ns.insert(ns.size(), pad - ns.size(), chr);
    } else {
        ns = ns.substr(0, pad);
    }
    std::cout << ns;
}

void pad_print(std::string s, long unsigned int pad, char chr = ' ') {
    pad_print_left(s, pad, chr);
}

std::optional<Id> get_most_recent_id_of_given_indent(int indent) {
    for (auto rit = tasks.rbegin(); rit != tasks.rend(); rit++) {
        if (rit->indent == indent) {
            return rit->id;
        }
    }
    return {};
}

void parse_file_to_tasks(std::string filepath) {
    std::ifstream databook_file{filepath};

    Task task{};
    std::string line;
    int id{0}; // od zera bo jestesmy programystami
    task.id = id++;
    while (std::getline(databook_file, line)) {
        {
            //scrap checked checkbox
            const std::string keyword_checkbox_checked{"[X]"};
            const auto pos = line.find(keyword_checkbox_checked);
            if (std::string::npos != pos) {
                continue; 
            }
        }
        {
            //scrap empty checkbox
            const std::string keyword_checkbox_empty{"[ ]"};
            const auto pos = line.find(keyword_checkbox_empty);
            if (std::string::npos != pos) {
                if (task.lines.size() > 0) {
                    tasks.push_back(task);
                    task = Task{};
                    task.id = id++;
                }
                assert(pos % 4 == 0);
                task.indent = pos/4; 
                if (task.indent != 0) {
                    task.parent = get_most_recent_id_of_given_indent(task.indent-1);
                    tasks[task.parent.value()].children.push_back(task.id);
                }
                line = line.erase(pos, keyword_checkbox_empty.size());
                trim(line);
            }
        }
        const auto scrapped_due = scrap_due(line);
        if (scrapped_due.has_value()) {
            task.has_dl = true;
            task.dl = scrapped_due.value();
        }
        const auto scrapped_prio = scrap_prio(line);
        if (scrapped_prio != Priority::NOT_SPECIFIED) {
            task.prio = scrapped_prio;
        }
        trim(line);
        line = castPolishLettersToASCII(line);
        task.lines.push_back(line);
        task.desc = line;
    }

    if (task.lines.size() > 0) {
        tasks.push_back(task);
        task = Task{};
        task.id = id++;
    }
    

}

int main(int argc, char **argv) {
    std::string program = argv[0];
    
    if (argc <= 0) {
        std::cerr 
            << "Usage: " 
            << program 
            << "[datebook.txt]"
            << std::endl
            << "ERROR: no datebook is provided"
            << std::endl;
        return(1);
    }
    assert(argc >= 1);
    std::string datebook_filepath = argv[1];
    parse_file_to_tasks(datebook_filepath);

    //////////////////////
    sort(tasks.begin(), tasks.end(), asc_pred);

    const std::string header_title_id = "ID";
    const std::string header_title_prio = "P";
    const std::string header_title_deps = "Deps";
    const std::string header_title_dl = "Due";
    const std::string header_title_desc = "Description";
    const std::string header_title_urg = "Urgency";

    int id_mx_width = header_title_id.size();
    int prio_mx_width = header_title_prio.size();
    int deps_mx_width = header_title_deps.size();
    int dl_mx_width = header_title_dl.size();
    int desc_mx_width = header_title_desc.size();
    int urg_mx_width = header_title_urg.size();

    for (const auto &task : tasks) {
        {
            int width = std::to_string(task.id).size();
            if (width > id_mx_width) {
                id_mx_width = width;
            }
        }

        if (Priority::NOT_SPECIFIED != task.prio) {
            int width = to_string(task.prio).size();
            if (width > prio_mx_width) {
                prio_mx_width = width;
            }
        }
        if (task.children.size() > 0) {
            std::string deps{}; // task depends on its children, aka subtasks
            for (const auto &child : task.children) {
                deps += std::to_string(child);
                deps += ", ";
            }
            int width = deps.size();
            if (width > deps_mx_width) {
                deps_mx_width = width;
            }
        }
        if (task.has_dl) {
            int width = std::to_string(task.dl).size();
            if (width > dl_mx_width) {
                dl_mx_width = width;
            }
        }
        {
            int width = task.desc.size();
    //        std::cout << "width = " << width << ' ' << task.desc << '\n';
            if (width > desc_mx_width) {
                desc_mx_width = width;
            }
        }
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1) << urgency(task);
            int width = stream.str().size();
            if (width > urg_mx_width) {
                urg_mx_width = width;
            }
        }
    }
    
    //prio_mx_width = 1;
    //desc_mx_width += 4;
    //desc_mx_width = 20;

    desc_mx_width = get_width_of_terminal() - id_mx_width - 1 - prio_mx_width - 1 - deps_mx_width - 1 - dl_mx_width - 1 - urg_mx_width - 1;

    pad_print(header_title_id, id_mx_width);
    std::cout << ' ';
    pad_print(header_title_prio, prio_mx_width);
    std::cout << ' ';
    pad_print(header_title_deps, deps_mx_width);
    std::cout << ' ';
    pad_print(header_title_dl, dl_mx_width);
    std::cout << ' ';
    pad_print_right(header_title_desc, desc_mx_width);
    std::cout << ' ';
    pad_print(header_title_urg, urg_mx_width);
    

    std::cout << std::endl;
    for (const auto &task : tasks) {
        pad_print(trim_copy(std::to_string(task.id)), id_mx_width);
        std::cout << ' ';

        if (task.prio != Priority::NOT_SPECIFIED) {
            pad_print(std::string{to_string(task.prio)}, prio_mx_width);
        } else {
            pad_print({"---"}, prio_mx_width);
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
            pad_print(deps, deps_mx_width);
        }
        std::cout << ' ';
        if (task.has_dl) {
            pad_print(std::to_string(task.dl), dl_mx_width);
        } else {
            pad_print({}, dl_mx_width);
        }
        std::cout << ' ';
        pad_print_right(trim_copy(task.desc), desc_mx_width);
        std::cout << ' ';
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1) << urgency(task);
            pad_print(stream.str(), urg_mx_width);
        }
#if 0
        std::cout << "\t\t";
        std::cout << task.id;
        std::cout << "\t\t";
        std::cout << task.parent.value_or(0);
#endif
        std::cout << std::endl;
    }
}