#include "../includes/Command.hpp"

CommandInfo::CommandInfo(const std::string& n, const std::string& d)
    : name(n), description(d) {
}

std::string CommandInfo::getName() const {
    return name;
}

std::string CommandInfo::getDescription() const {
    return description;
}
