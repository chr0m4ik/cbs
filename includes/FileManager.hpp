#ifndef _FILE_MANAGER_HPP_
#define _FILE_MANAGER_HPP_

#include "Utility.hpp"

// Менеджер операций с файлами
class FileManager {
public:
    // ls команда
    static void listFiles(const std::vector<std::string>& args);
    
    // pwd команда
    static void printWorkingDirectory();
    
    // cd команда
    static bool changeDirectory(const std::string& target);
    
    // mkdir команда
    static void makeDirectory(const std::vector<std::string>& args);
    
    // rm команда
    static void removeFile(const std::vector<std::string>& args);
    
    // cp команда
    static void copyFile(const std::vector<std::string>& args);
    
    // mv команда
    static void moveFile(const std::vector<std::string>& args);
    
    // touch команда
    static void touchFile(const std::vector<std::string>& args);
    
    // cat команда
    static void catFile(const std::vector<std::string>& args);
    
    // head команда
    static void headFile(const std::vector<std::string>& args);
    
    // tail команда
    static void tailFile(const std::vector<std::string>& args);

private:
    static void recursiveRemove(const fs::path& path);
    static void recursiveCopy(const fs::path& src, const fs::path& dst);
};

#endif // _FILE_MANAGER_HPP_
