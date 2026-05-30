#ifndef _ALIAS_MANAGER_HPP_
#define _ALIAS_MANAGER_HPP_

#include "Utility.hpp"

// Менеджер алиасов
class AliasManager {
public:
    AliasManager();
    bool loadAliases(const std::string& path);
    std::string expandAlias(const std::string& command) const;
    bool hasAlias(const std::string& command) const;

private:
    std::map<std::string, std::string> aliases;
};

#endif // _ALIAS_MANAGER_HPP_
