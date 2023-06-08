int main(int argc, char **argv) {
    std::string program = argv[0];
    if (argc <= 0) {
        std::cerr 
            << "Usage: " 
            << program 
            << "[datebook.txt|directory_full_of_datebooks]"
            << std::endl
            << "ERROR: neither datebook nor directory is provided"
            << std::endl;
        return(1);
    }
 //   assert(argc >= 2);
    std::string path = argv[1];

    if (fs::is_regular_file(path)) {
        parse_file_to_tasks(path);
    } else if (fs::is_directory(path)) {
        parse_dir_to_tasks(path);
    } else {
        std::cerr 
            << "Usage: " 
            << program 
            << "[datebook.txt|directory_full_of_datebooks]"
            << std::endl
            << "ERROR: neither datebook nor directory is provided"
            << std::endl;
        return(1);
    }

    //////////////////////
    sort(tasks.begin(), tasks.end(), asc_pred);
    auto table = tasks_to_output_table();
    print_table(table);
}
