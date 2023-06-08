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
            return -3.9f;
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
    fs::path file;
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
    assert(!"unreachable");
    return {};
}

int id{}; // od zera bo jestesmy programystami
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

using Entries = std::vector<std::string>;

struct Column_Of_Table {
    Title title;
    long unsigned int width;
};

using Columns = std::map<Title, Column_Of_Table>;

struct Output_Table {
    Columns cols;
};

Output_Table tasks_to_output_table() {
    Output_Table res{};
    res.cols[Title::ID] = Column_Of_Table{Title::ID, to_string(Title::ID).size()};
    res.cols[Title::PRIO] = Column_Of_Table{Title::PRIO, to_string(Title::PRIO).size()};
    res.cols[Title::PARENT] = {Title::PARENT, to_string(Title::PARENT).size()};
    res.cols[Title::DEPS] = {Title::DEPS, to_string(Title::DEPS).size()};
    res.cols[Title::DL] = {Title::DL, to_string(Title::DL).size()};
    res.cols[Title::DESC] = {Title::DESC, to_string(Title::DESC).size()};
    res.cols[Title::FILENAME] = {Title::FILENAME, to_string(Title::FILENAME).size()};
    res.cols[Title::URG] = {Title::URG, to_string(Title::URG).size()};

    for (const auto &task : tasks) {
        {

            long unsigned int width = std::to_string(task.id).size();
            if (width > res.cols[Title::ID].width) {
                res.cols[Title::ID].width = width;
            }
        }

        if (Priority::NOT_SPECIFIED != task.prio) {
            long unsigned int width = to_string(task.prio).size();
            if (width > res.cols[Title::PRIO].width) {
                res.cols[Title::PRIO].width = width;
            }
        }
        if (task.parent.has_value()) {
            long unsigned int width = std::to_string(task.parent.value()).size();
            if (width > res.cols[Title::PARENT].width) {
                res.cols[Title::PARENT].width = width;
            }
        }
        if (task.children.size() > 0) {
            std::string deps{}; // task depends on its children, aka subtasks
            for (const auto &child : task.children) {
                deps += std::to_string(child);
                deps += ", ";
            }
            long unsigned int width = deps.size();
            if (width > res.cols[Title::DEPS].width) {
                res.cols[Title::DEPS].width = width;
            }
        }
        if (task.has_dl) {
            long unsigned int width = std::to_string(task.dl).size();
            if (width > res.cols[Title::DL].width) {
                res.cols[Title::DL].width = width;
            }
        }
        {
            long unsigned int width = task.desc.size();
            if (width > res.cols[Title::DESC].width) {
                res.cols[Title::DESC].width = width;
            }
        }
        {            
            long unsigned int width = std::string{task.file.stem()}.size();
            if (width > res.cols[Title::FILENAME].width) {
                res.cols[Title::FILENAME].width = width;
            }
        }
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1) << urgency(task);
            long unsigned int width = stream.str().size();
            if (width > res.cols[Title::URG].width) {
                res.cols[Title::URG].width = width;
            }
        }
    }
    return res;
}   

void print_table(Output_Table table) {

    table.cols[Title::PARENT].width = 3;

    table.cols[Title::DESC].width = get_width_of_terminal() - table.cols[Title::ID].width - 1 - table.cols[Title::PRIO].width - 1 - table.cols[Title::PARENT].width - 1 - table.cols[Title::DEPS].width - 1 - table.cols[Title::DL].width - 1 - table.cols[Title::FILENAME].width - 1 - table.cols[Title::URG].width - 4;

    pad_print(to_string(Title::ID), table.cols[Title::ID].width);
    std::cout << ' ';
    pad_print(to_string(Title::PRIO), table.cols[Title::PRIO].width);
    std::cout << ' ';
    pad_print(to_string(Title::PARENT), table.cols[Title::PARENT].width);
    std::cout << ' ';
    pad_print(to_string(Title::DEPS), table.cols[Title::DEPS].width);
    std::cout << ' ';
    pad_print(to_string(Title::DL), table.cols[Title::DL].width);
    std::cout << ' ';
    pad_print_right(to_string(Title::DESC), table.cols[Title::DESC].width);
    std::cout << ' ';
    pad_print(to_string(Title::FILENAME), table.cols[Title::FILENAME].width);    
    std::cout << ' ';
    pad_print(to_string(Title::URG), table.cols[Title::URG].width);    
    std::cout << '\n';


    for (const auto &task : tasks) {
        pad_print(trim_copy(std::to_string(task.id)), table.cols[Title::ID].width);
        std::cout << ' ';

        if (task.prio != Priority::NOT_SPECIFIED) {
            pad_print(std::string{to_string(task.prio)}, table.cols[Title::PRIO].width);
        } else {
            pad_print({}, table.cols[Title::PRIO].width);
        }
        if (task.parent.has_value()) {
            pad_print(std::to_string(task.parent.value()), table.cols[Title::PARENT].width);
        } else {
            pad_print({}, table.cols[Title::PARENT].width);
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
            pad_print(deps, table.cols[Title::DEPS].width);
        }
        std::cout << ' ';
        if (task.has_dl) {
            pad_print(std::to_string(task.dl), table.cols[Title::DL].width);
        } else {
            pad_print({}, table.cols[Title::DL].width);
        }
        std::cout << ' ';
        pad_print_right(trim_copy(task.desc), table.cols[Title::DESC].width);
        std::cout << ' ';
        pad_print(trim_copy(task.file.stem()), table.cols[Title::FILENAME].width);
        std::cout << ' ';
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1) << urgency(task);
            pad_print(stream.str(), table.cols[Title::URG].width);
        }
        std::cout << std::endl;
    }
}
