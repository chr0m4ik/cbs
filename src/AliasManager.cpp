#include "../includes/AliasManager.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

AliasManager::AliasManager() {
}

bool AliasManager::loadAliases(const std::string& path) {
    if (!fileExists(path)) {
        return false;
    }

    try {
        std::ifstream file(path);
        json data = json::parse(file);
        
        for (auto& [key, value] : data.items()) {
            aliases[key] = value.get<std::string>();
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading aliases: " << e.what() << "\n";
        return false;
    }
}

std::string AliasManager::expandAlias(const std::string& command) const {
    auto it = aliases.find(command);
    if (it != aliases.end()) {
        return it->second;
    }
    return command;
}

bool AliasManager::hasAlias(const std::string& command) const {
    return aliases.find(command) != aliases.end();
}
