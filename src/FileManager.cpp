#include "../includes/FileManager.hpp"

void FileManager::printWorkingDirectory() {
    try {
        std::cout << fs::current_path().string() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "pwd: " << e.what() << "\n";
    }
}

bool FileManager::changeDirectory(const std::string& target) {
    try {
        fs::current_path(target);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "cd: " << e.what() << "\n";
        return false;
    }
}

void FileManager::makeDirectory(const std::vector<std::string>& args) {
    bool recursive = false;
    std::vector<std::string> paths;

    for (const auto& arg : args) {
        if (arg == "-p") {
            recursive = true;
        } else if (arg[0] == '-') {
            std::cerr << "mkdir: invalid option -- '" << arg << "'\n";
            return;
        } else {
            paths.push_back(arg);
        }
    }

    for (const auto& path : paths) {
        try {
            if (recursive) {
                fs::create_directories(path);
            } else {
                fs::create_directory(path);
            }
        } catch (const std::exception& e) {
            std::cerr << "mkdir: cannot create directory '" << path << "': " << e.what() << "\n";
        }
    }
}

void FileManager::recursiveRemove(const fs::path& path) {
    try {
        fs::remove_all(path);
    } catch (const std::exception& e) {
        std::cerr << "rm: cannot remove '" << path << "': " << e.what() << "\n";
    }
}

void FileManager::removeFile(const std::vector<std::string>& args) {
    bool recursive = false;
    bool force = false;
    std::vector<std::string> paths;

    for (const auto& arg : args) {
        if (arg == "-r") {
            recursive = true;
        } else if (arg == "-f") {
            force = true;
        } else if (arg[0] == '-') {
            std::cerr << "rm: invalid option -- '" << arg << "'\n";
            return;
        } else {
            paths.push_back(arg);
        }
    }

    for (const auto& path : paths) {
        if (!fs::exists(path)) {
            if (!force) {
                std::cerr << "rm: cannot remove '" << path << "': No such file or directory\n";
            }
            continue;
        }

        if (fs::is_directory(path) && !recursive) {
            std::cerr << "rm: cannot remove '" << path << "': Is a directory (use -r)\n";
            continue;
        }

        recursiveRemove(path);
    }
}

void FileManager::recursiveCopy(const fs::path& src, const fs::path& dst) {
    try {
        if (fs::is_directory(src)) {
            fs::create_directories(dst);
            for (const auto& entry : fs::directory_iterator(src)) {
                fs::path newDst = dst / entry.path().filename();
                recursiveCopy(entry.path(), newDst);
            }
        } else {
            fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
        }
    } catch (const std::exception& e) {
        std::cerr << "cp: cannot copy '" << src << "' to '" << dst << "': " << e.what() << "\n";
    }
}

void FileManager::copyFile(const std::vector<std::string>& args) {
    bool recursive = false;
    std::vector<std::string> sources;
    std::string destination;

    for (size_t i = 0; i < args.size(); ++i) {
        const auto& arg = args[i];
        if (arg == "-r") {
            recursive = true;
        } else if (arg[0] == '-') {
            std::cerr << "cp: invalid option -- '" << arg << "'\n";
            return;
        } else {
            if (i == args.size() - 1) {
                destination = arg;
            } else {
                sources.push_back(arg);
            }
        }
    }

    if (sources.empty() || destination.empty()) {
        std::cerr << "cp: missing file operand\n";
        return;
    }

    for (const auto& src : sources) {
        if (!fs::exists(src)) {
            std::cerr << "cp: cannot stat '" << src << "': No such file or directory\n";
            continue;
        }

        if (fs::is_directory(src) && !recursive) {
            std::cerr << "cp: omitting directory '" << src << "' (use -r)\n";
            continue;
        }

        fs::path dstPath = destination;
        if (sources.size() > 1 && fs::is_directory(dstPath)) {
            dstPath /= fs::path(src).filename();
        }

        recursiveCopy(src, dstPath);
    }
}

void FileManager::moveFile(const std::vector<std::string>& args) {
    std::vector<std::string> sources;
    std::string destination;

    for (size_t i = 0; i < args.size(); ++i) {
        const auto& arg = args[i];
        if (arg[0] == '-') {
            std::cerr << "mv: invalid option -- '" << arg << "'\n";
            return;
        } else {
            if (i == args.size() - 1) {
                destination = arg;
            } else {
                sources.push_back(arg);
            }
        }
    }

    if (sources.empty() || destination.empty()) {
        std::cerr << "mv: missing file operand\n";
        return;
    }

    for (const auto& src : sources) {
        if (!fs::exists(src)) {
            std::cerr << "mv: cannot stat '" << src << "': No such file or directory\n";
            continue;
        }

        fs::path dstPath = destination;
        if (sources.size() > 1 && fs::is_directory(dstPath)) {
            dstPath /= fs::path(src).filename();
        }

        try {
            fs::rename(src, dstPath);
        } catch (const std::exception& e) {
            std::cerr << "mv: cannot move '" << src << "' to '" << dstPath << "': " << e.what() << "\n";
        }
    }
}

void FileManager::touchFile(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        try {
            if (fs::exists(arg)) {
                // Обновляем время модификации, открыв файл в режиме append
                std::ofstream file(arg, std::ios::app);
                file.close();
            } else {
                // Создаём новый пустой файл
                std::ofstream file(arg);
                file.close();
            }
        } catch (const std::exception& e) {
            std::cerr << "touch: cannot access '" << arg << "': " << e.what() << "\n";
        }
    }
}

void FileManager::catFile(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "cat: missing file operand\n";
        return;
    }

    for (const auto& arg : args) {
        try {
            std::ifstream file(arg);
            if (!file.is_open()) {
                std::cerr << "cat: cannot open '" << arg << "': No such file or directory\n";
                continue;
            }
            std::string line;
            while (std::getline(file, line)) {
                std::cout << line << "\n";
            }
            file.close();
        } catch (const std::exception& e) {
            std::cerr << "cat: " << arg << ": " << e.what() << "\n";
        }
    }
}

void FileManager::headFile(const std::vector<std::string>& args) {
    int numLines = 10;
    std::vector<std::string> files;

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "-n" && i + 1 < args.size()) {
            if (isNumber(args[i + 1])) {
                numLines = std::stoi(args[i + 1]);
                i++;
            } else {
                std::cerr << "head: invalid number of lines: " << args[i + 1] << "\n";
                return;
            }
        } else if (args[i][0] == '-') {
            std::cerr << "head: invalid option -- '" << args[i] << "'\n";
            return;
        } else {
            files.push_back(args[i]);
        }
    }

    if (files.empty()) {
        std::cerr << "head: missing file operand\n";
        return;
    }

    for (size_t idx = 0; idx < files.size(); ++idx) {
        const auto& file = files[idx];
        if (files.size() > 1) {
            if (idx > 0) std::cout << "\n";
            std::cout << "==> " << file << " <==\n";
        }

        try {
            std::ifstream ifs(file);
            if (!ifs.is_open()) {
                std::cerr << "head: " << file << ": No such file or directory\n";
                continue;
            }
            std::string line;
            int count = 0;
            while (std::getline(ifs, line) && count < numLines) {
                std::cout << line << "\n";
                count++;
            }
            ifs.close();
        } catch (const std::exception& e) {
            std::cerr << "head: " << file << ": " << e.what() << "\n";
        }
    }
}

void FileManager::tailFile(const std::vector<std::string>& args) {
    int numLines = 10;
    bool follow = false;
    std::vector<std::string> files;

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "-n" && i + 1 < args.size()) {
            if (isNumber(args[i + 1])) {
                numLines = std::stoi(args[i + 1]);
                i++;
            } else {
                std::cerr << "tail: invalid number of lines: " << args[i + 1] << "\n";
                return;
            }
        } else if (args[i] == "-f") {
            follow = true;
        } else if (args[i][0] == '-') {
            std::cerr << "tail: invalid option -- '" << args[i] << "'\n";
            return;
        } else {
            files.push_back(args[i]);
        }
    }

    if (files.empty()) {
        std::cerr << "tail: missing file operand\n";
        return;
    }

    for (size_t idx = 0; idx < files.size(); ++idx) {
        const auto& file = files[idx];
        if (files.size() > 1) {
            if (idx > 0) std::cout << "\n";
            std::cout << "==> " << file << " <==\n";
        }

        try {
            std::ifstream ifs(file);
            if (!ifs.is_open()) {
                std::cerr << "tail: " << file << ": No such file or directory\n";
                continue;
            }
            std::vector<std::string> lines;
            std::string line;
            while (std::getline(ifs, line)) {
                lines.push_back(line);
            }
            ifs.close();

            size_t start = (lines.size() > static_cast<size_t>(numLines)) 
                ? lines.size() - numLines 
                : 0;
            for (size_t i = start; i < lines.size(); ++i) {
                std::cout << lines[i] << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "tail: " << file << ": " << e.what() << "\n";
        }
    }
}

void FileManager::listFiles(const std::vector<std::string>& args) {
    std::vector<std::string> paths;
    bool longFormat = false;

    for (const auto& arg : args) {
        if (arg == "-l") {
            longFormat = true;
        } else if (arg[0] == '-') {
            // ignore other options for now
        } else {
            paths.push_back(arg);
        }
    }

    if (paths.empty()) {
        paths.push_back(".");
    }

    for (const auto& path : paths) {
        try {
            if (!fs::exists(path)) {
                std::cerr << "ls: cannot access '" << path << "': No such file or directory\n";
                continue;
            }

            if (fs::is_regular_file(path)) {
                std::cout << fs::path(path).filename().string() << "\n";
                continue;
            }

            if (paths.size() > 1) {
                std::cout << path << ":\n";
            }

            std::vector<fs::directory_entry> entries;
            for (const auto& entry : fs::directory_iterator(path)) {
                entries.push_back(entry);
            }
            std::sort(entries.begin(), entries.end());

            for (const auto& entry : entries) {
                const auto& p = entry.path();
                std::string name = p.filename().string();

                if (longFormat) {
                    auto status = fs::status(p);
                    auto size = fs::is_regular_file(p) ? fs::file_size(p) : 0;
                    auto mtime = fs::last_write_time(p);
                    
                    char type = fs::is_directory(p) ? 'd' : '-';
                    std::cout << type << " "
                              << std::setw(10) << size << " "
                              << formatTime(mtime) << " "
                              << name << "\n";
                } else {
                    std::cout << std::setw(20) << std::left << name;
                }
            }

            if (!longFormat && !entries.empty()) {
                std::cout << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "ls: " << path << ": " << e.what() << "\n";
        }
    }
}
