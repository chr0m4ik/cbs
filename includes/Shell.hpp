#ifndef _SHELL_HPP_
#define _SHELL_HPP_

#include "Utility.hpp"
#include "Command.hpp"
#include "FileManager.hpp"
#include "AliasManager.hpp"

// Главный класс терминала
class Shell {
public:
    Shell();
    ~Shell();
    void run();

private:
    AliasManager aliasManager;
    std::string currentDirectory;

    // Встроенные команды
    bool handleBuiltin(const std::string& cmd, const std::vector<std::string>& args);
    
    // Вспомогательные методы
    std::string getPrompt() const;
    std::vector<CommandInfo> getAvailableCommands() const;
    void executeExternalScript(const std::string& cmd, const std::vector<std::string>& args);
    std::string findCommandInPath(const std::string& cmd) const;
    std::string getShellDirectory() const;
    std::string getAliasesPath() const;
};

#endif // _SHELL_HPP_
