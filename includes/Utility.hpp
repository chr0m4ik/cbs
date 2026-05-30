#ifndef _UTILITY_HPP_
#define _UTILITY_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <algorithm>
#include <chrono>
#include <iomanip>

namespace fs = std::filesystem;

// Очистка экрана
inline void clearScreen() {
    if (system("cls") != 0) {
        system("clear");
    }
}

// Разделение строки по разделителю
inline std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Разделение строки на аргументы (с учётом кавычек)
inline std::vector<std::string> parseCommandLine(const std::string& line) {
    std::vector<std::string> args;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < line.length(); ++i) {
        char ch = line[i];
        if (ch == '"') {
            inQuotes = !inQuotes;
        } else if (ch == ' ' && !inQuotes) {
            if (!current.empty()) {
                args.push_back(current);
                current.clear();
            }
        } else {
            current += ch;
        }
    }
    if (!current.empty()) {
        args.push_back(current);
    }
    return args;
}

// Проверка, существует ли файл
inline bool fileExists(const std::string& path) {
    return fs::exists(path);
}

// Форматирование размера файла в читаемый вид
inline std::string formatFileSize(uint64_t size) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    double s = static_cast<double>(size);
    int unitIdx = 0;
    while (s >= 1024.0 && unitIdx < 4) {
        s /= 1024.0;
        unitIdx++;
    }
    std::stringstream ss;
    if (unitIdx == 0) {
        ss << size << " " << units[unitIdx];
    } else {
        ss << std::fixed << std::setprecision(1) << s << " " << units[unitIdx];
    }
    return ss.str();
}

// Форматирование времени модификации
inline std::string formatTime(const fs::file_time_type& time) {
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        time - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    auto tt = std::chrono::system_clock::to_time_t(sctp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&tt), "%b %d %H:%M");
    return ss.str();
}

// Проверка, является ли строка числом
inline bool isNumber(const std::string& str) {
    if (str.empty()) return false;
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

// Преобразование к нижнему регистру
inline std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

#endif // _UTILITY_HPP_
