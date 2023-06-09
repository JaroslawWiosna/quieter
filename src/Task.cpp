#include "Task.hpp"

////////////////////////////////////////////////////////////////////////

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

std::optional<Day> scrap_due(std::string &s) {
    const auto pos = s.find(keyword_due);
    if (std::string::npos != pos) {
        int result = diff_in_days(s.substr(s.find(keyword_due)+keyword_due.size(), std::string{"2000-00-00"}.size()));
        s = s.erase(pos, keyword_due.size()+std::string{"2000-00-00"}.size());
        return result;
    }
    return {}; 
}

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
    assert(!"unreachable");
    return {};
}

void parse_file_to_tasks(fs::path filepath) {
    std::ifstream databook_file{filepath};

    Task task{};
    task.file = filepath;
    std::string line;
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
                    task.file = filepath;
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
    }
}

void parse_dir_to_tasks(fs::path dirpath) {
    for(const auto& entry : fs::directory_iterator(dirpath)) {
        if (fs::is_regular_file(entry)) {
            parse_file_to_tasks(entry);
        }
    }
}

std::string to_string(Title title) {
    switch(title) {
        case Title::ID:
            return "ID";
        case Title::PRIO:
            return "P";
        case Title::PARENT:
            return "Par";
        case Title::DEPS:
            return "Deps";
        case Title::DL:
            return "Due";
        case Title::DESC:
            return "Description";
        case Title::FILENAME:
            return "Filename";
        case Title::URG:
            return "Urg";
        default:
            throw;
    }
    throw;
}
