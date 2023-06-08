int main(int argc, char **argv) {
    Opts opts = args_to_opts(parse_argv(argc, argv));

    if (fs::is_regular_file(opts.path)) {
        parse_file_to_tasks(opts.path);
    } else if (fs::is_directory(opts.path)) {
        parse_dir_to_tasks(opts.path);
    } else {
        std::cerr 
            << "Usage: " 
            << opts.program 
            << "[datebook.txt|directory_full_of_datebooks]"
            << std::endl
            << "ERROR: path is not datebook/directory"
            << std::endl;
        return(1);
    }

    //////////////////////
    sort(tasks.begin(), tasks.end(), asc_pred);
    auto table = tasks_to_output_table();
    print_table(table, opts);
}
