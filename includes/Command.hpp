#ifndef _COMMAND_HPP_
#define _COMMAND_HPP_

#include "Utility.hpp"

// Информация о команде
class CommandInfo {
public:
    CommandInfo(const std::string& n, const std::string& d = "");
    std::string getName() const;
    std::string getDescription() const;

private:
    std::string name;
    std::string description;
};

// Интерфейс для внешних команд
class ExternalCommand {
public:
    virtual ~ExternalCommand() = default;
    virtual bool execute(const std::vector<std::string>& args) = 0;
    virtual std::string getName() const = 0;
};

#endif // _COMMAND_HPP_
