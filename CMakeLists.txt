cmake_minimum_required(VERSION 3.10)
project(CBSShell)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем интерпретатор JSON
find_package(nlohmann_json 3.2.0 QUIET)
if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
    FetchContent_MakeAvailable(json)
endif()

# Основной исполняемый файл
add_executable(cbs_shell
    src/main.cpp
    src/Shell.cpp
    src/Command.cpp
    src/FileManager.cpp
    src/AliasManager.cpp
)

target_link_libraries(cbs_shell PRIVATE nlohmann_json::nlohmann_json)

# Добавляем директорию includes
target_include_directories(cbs_shell PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)

# Установка опций компилятора для Windows
if(MSVC)
    target_compile_options(cbs_shell PRIVATE /W4)
    target_compile_definitions(cbs_shell PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(cbs_shell PRIVATE -Wall -Wextra -Wpedantic)
endif()
