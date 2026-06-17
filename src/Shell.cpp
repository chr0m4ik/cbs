#include "../includes/Shell.hpp"
#include <thread>
#include <Windows.h>

Shell::Shell() {
    currentDirectory = fs::current_path().string();

    std::string aliasPath = getAliasesPath();
    aliasManager.loadAliases(aliasPath);

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

Shell::~Shell() {
}

std::string Shell::getShellDirectory() const {
    const char* env = std::getenv("CBS_SHELL_DIR");
    if (env) {
        return env;
    }
    
    // Получаем директорию исполняемого файла
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string exePath(buffer);
    size_t pos = exePath.find_last_of("\\/");
    if (pos != std::string::npos) {
        return exePath.substr(0, pos) + "\\Shell";
    }
    return "Shell";
}

std::string Shell::getAliasesPath() const {
    const char* env = std::getenv("CBS_ALIAS_FILE");
    if (env) {
        return env;
    }
    
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string exePath(buffer);
    size_t pos = exePath.find_last_of("\\/");
    if (pos != std::string::npos) {
        return exePath.substr(0, pos) + "\\aliases.json";
    }
    return "aliases.json";
}

std::string Shell::getPrompt() const {
    return "(" + currentDirectory + ")> ";
}


// Полное описание встроенных команд (для флага -h / --help)
std::string getBuiltinHelp(const std::string& cmd) {
    if (cmd == "ls") {
        return "ls - вывод списка файлов\n"
               "    -l: подробный вывод (тип, размер, дата)\n";
    }
    if (cmd == "pwd") {
        return "pwd - показать текущую папку\n";
    }
    if (cmd == "cd") {
        return "cd <папка> - перейти в папку\n"
               "    без аргумента - перейти в домашнюю папку\n";
    }
    if (cmd == "mkdir") {
        return "mkdir <папка...> - создать папки\n"
               "    -p: создавать вложенные папки целиком\n";
    }
    if (cmd == "rm") {
        return "rm <путь...> - удалить файлы или папки\n"
               "    -r: удалять папки рекурсивно\n"
               "    -f: не ругаться, если файла нет\n";
    }
    if (cmd == "cp") {
        return "cp <источник...> <назначение> - копировать\n"
               "    -r: копировать папки рекурсивно\n";
    }
    if (cmd == "mv") {
        return "mv <источник...> <назначение> - переместить файлы\n";
    }
    if (cmd == "rename") {
        return "rename <старое_имя> <новое_имя> - переименовать файл или папку\n";
    }
    if (cmd == "touch") {
        return "touch <файл...> - создать пустой файл или обновить время\n";
    }
    if (cmd == "cat") {
        return "cat <файл...> - вывести содержимое файлов\n";
    }
    if (cmd == "head") {
        return "head <файл...> - вывести первые строки файла\n"
               "    -n <N>: сколько строк выводить (по умолчанию 10)\n";
    }
    if (cmd == "tail") {
        return "tail <файл...> - вывести последние строки файла\n"
               "    -n <N>: сколько строк выводить (по умолчанию 10)\n"
               "    -f: следить за файлом (пока не реализовано)\n";
    }
    if (cmd == "cls") {
        return "cls - очистить экран\n";
    }
    if (cmd == "help") {
        return "help - показать список доступных команд\n";
    }
    if (cmd == "exit" || cmd == "quit") {
        return "exit / quit - выйти из терминала\n";
    }
    return "";
}

// Ищет скрипт команды в папке Shell (.bat / .ps1 / .py)
std::string findShellScript(const std::string& shellDir, const std::string& cmd) {
    std::vector<std::string> extensions = {".bat", ".ps1", ".py"};
    for (const auto& ext : extensions) {
        std::string scriptPath = shellDir + "\\" + cmd + ext;
        if (fileExists(scriptPath)) {
            return scriptPath;
        }
    }
    return "";
}

// Читает скрипт и выводит все строки с пометкой //FULL_INFO:
void printScriptFullInfo(const std::string& scriptPath) {
    std::ifstream file(scriptPath);
    if (!file.is_open()) {
        std::cerr << "Cannot open script: " << scriptPath << "\n";
        return;
    }

    std::string marker = "//FULL_INFO:";
    std::string line;
    bool found = false;

    while (std::getline(file, line)) {
        size_t pos = line.find(marker);
        if (pos != std::string::npos) {
            std::string info = line.substr(pos + marker.length());
            size_t start = info.find_first_not_of(" \t");
            if (start != std::string::npos) {
                info = info.substr(start);
            }
            std::cout << info << "\n";
            found = true;
        }
    }

    if (!found) {
        std::cout << "Нет описания для этой команды.\n";
    }
}


std::vector<CommandInfo> getShellCommands(const std::string& shellDir) {
    std::vector<CommandInfo> commands;
    
    try {
        if (!fs::is_directory(shellDir)) {
            return commands;
        }
        
        for (const auto& entry : fs::directory_iterator(shellDir)) {
            if (fs::is_regular_file(entry)) {
                std::string ext = entry.path().extension().string();
                if (ext == ".bat" || ext == ".ps1" || ext == ".py") {
                    std::string name = entry.path().stem().string();
                    commands.push_back(CommandInfo(name, ""));
                }
            }
        }
        
        // Сортировка команд
        std::sort(commands.begin(), commands.end(),
            [](const CommandInfo& a, const CommandInfo& b) {
                return a.getName() < b.getName();
            });
    } catch (const std::exception& e) {
        std::cerr << "Error reading shell directory: " << e.what() << "\n";
    }
    
    return commands;
}

std::vector<CommandInfo> Shell::getAvailableCommands() const {
    std::vector<CommandInfo> commands;
    
    // Встроенные команды
    commands.push_back(CommandInfo("help", "show this help"));
    commands.push_back(CommandInfo("exit", "exit the shell"));
    commands.push_back(CommandInfo("quit", "exit the shell"));
    commands.push_back(CommandInfo("pwd", "print working directory"));
    commands.push_back(CommandInfo("cd", "change directory"));
    commands.push_back(CommandInfo("ls", "list files"));
    commands.push_back(CommandInfo("cls", "clear the screen"));
    commands.push_back(CommandInfo("mkdir", "create directories"));
    commands.push_back(CommandInfo("rm", "remove files/directories"));
    commands.push_back(CommandInfo("cp", "copy files/directories"));
    commands.push_back(CommandInfo("rename", "rename a file or directory"));
    commands.push_back(CommandInfo("mv", "move/rename files/directories"));
    commands.push_back(CommandInfo("touch", "create or update file"));
    commands.push_back(CommandInfo("cat", "concatenate and print files"));
    commands.push_back(CommandInfo("head", "output first N lines"));
    commands.push_back(CommandInfo("tail", "output last N lines"));
    
    // Внешние команды из Shell директории
    std::string shellDir = getShellDirectory();
    auto shellCommands = getShellCommands(shellDir);
    commands.insert(commands.end(), shellCommands.begin(), shellCommands.end());
    
    return commands;
}

std::string Shell::findCommandInPath(const std::string& cmd) const {
    const char* pathEnv = std::getenv("PATH");
    if (!pathEnv) return "";
    
    std::string extensions[] = {".exe", ".bat", ".cmd", ".com", ".ps1", ".py"};
    std::string pathStr(pathEnv);
    std::vector<std::string> paths = splitString(pathStr, ';');
    
    for (const auto& pathDir : paths) {
        for (const auto& ext : extensions) {
            std::string fullPath = pathDir + "\\" + cmd + ext;
            if (fileExists(fullPath)) {
                return fullPath;
            }
        }
    }
    
    return "";
}

// Запуск внешнего скрипта или команды
void Shell::executeExternalScript(const std::string& cmd, const std::vector<std::string>& args) {
    std::string shellDir = getShellDirectory();
    std::vector<std::string> extensions = {".bat", ".ps1", ".py"};

    for (const auto& ext : extensions) {
        std::string scriptPath = shellDir + "\\" + cmd + ext;

        if (!fileExists(scriptPath)) continue;

        std::string command;
        if (ext == ".bat") {
            command = "cmd /c \"" + scriptPath + "\"";
        } else if (ext == ".ps1") {
            command = "powershell -File \"" + scriptPath + "\"";
        } else if (ext == ".py") {
            command = "python \"" + scriptPath + "\"";
        }

        for (const auto& arg : args) {
            command += " \"" + arg + "\"";
        }

        command = "\"" + command + "\"";

        int result = system(command.c_str());
        if (result != 0) {
            std::cerr << cmd << " exited with code " << result << "\n";
        }
        return;
    }

    std::string pathCmd = findCommandInPath(cmd);
    if (!pathCmd.empty()) {
        std::string command = "\"" + pathCmd + "\"";
        for (const auto& arg : args) {
            command += " \"" + arg + "\"";
        }

        command = "\"" + command + "\"";

        int result = system(command.c_str());
        if (result != 0) {
            std::cerr << "Command exited with code " << result << "\n";
        }
        return;
    }

    std::cerr << "Unknown command: " << cmd << "\n";
}

bool Shell::handleBuiltin(const std::string& cmd, const std::vector<std::string>& args) {
    if (cmd == "exit" || cmd == "quit") {
        std::cout << "exit\n";
        exit(0);
    }
    else if (cmd == "help") {
        std::cout << "Available commands:\n";
        auto commands = getAvailableCommands();
        for (const auto& info : commands) {
            std::cout << "  " << info.getName();
            if (!info.getDescription().empty()) {
                std::cout << " - " << info.getDescription();
            }
            std::cout << "\n";
        }
        return true;
    }
    else if (cmd == "pwd") {
        FileManager::printWorkingDirectory();
        return true;
    }
    else if (cmd == "ls") {
        FileManager::listFiles(args);
        return true;
    }
    else if (cmd == "cls") {
        clearScreen();
        return true;
    }
    else if (cmd == "rename") {
        FileManager::renameFile(args);
        return true;
    }
    else if (cmd == "cd") {
        if (args.empty()) {
            // cd без аргументов -> домой
            char* home = std::getenv("USERPROFILE");
            if (home) {
                FileManager::changeDirectory(home);
                currentDirectory = home;
            }
        } else {
            std::string target = args[0];
            if (FileManager::changeDirectory(target)) {
                currentDirectory = fs::current_path().string();
            }
        }
        return true;
    }
    else if (cmd == "mkdir") {
        FileManager::makeDirectory(args);
        return true;
    }
    else if (cmd == "rm") {
        FileManager::removeFile(args);
        return true;
    }
    else if (cmd == "cp") {
        FileManager::copyFile(args);
        return true;
    }
    else if (cmd == "mv") {
        FileManager::moveFile(args);
        return true;
    }
    else if (cmd == "touch") {
        FileManager::touchFile(args);
        return true;
    }
    else if (cmd == "cat") {
        FileManager::catFile(args);
        return true;
    }
    else if (cmd == "head") {
        FileManager::headFile(args);
        return true;
    }
    else if (cmd == "tail") {
        FileManager::tailFile(args);
        return true;
    }
    
    return false;
}

void Shell::run() {
    std::cout << "Welcome to cbs shell!\n";
    std::cout << "Type help for commands, exit to quit.\n";
    
    std::string line;
    while (true) {
        std::cout << getPrompt();
        std::getline(std::cin, line);
        
        if (line.empty()) {
            continue;
        }
        
        // Разбираем команду и аргументы
        auto parts = parseCommandLine(line);
        if (parts.empty()) {
            continue;
        }
        
        std::string cmd = parts[0];
        std::vector<std::string> cmdArgs(parts.begin() + 1, parts.end());
        
        // Проверяем, есть ли алиас
        if (aliasManager.hasAlias(cmd)) {
            std::string expandedAlias = aliasManager.expandAlias(cmd);
            auto aliasParts = parseCommandLine(expandedAlias);
            if (!aliasParts.empty()) {
                cmd = aliasParts[0];
                cmdArgs.insert(cmdArgs.begin(), aliasParts.begin() + 1, aliasParts.end());
            }
        }
        
        // Пытаемся выполнить встроенную команду
        if (handleBuiltin(cmd, cmdArgs)) {
            continue;
        }
        
        // Пытаемся выполнить внешний скрипт или команду
        executeExternalScript(cmd, cmdArgs);
    }
}
