#include <filesystem>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace fs = std::filesystem;

namespace gmloader {
    struct config {
        std::string save_dir;
        std::string apk_path;
        bool show_cursor;
        bool disable_controller;
        bool disable_depth;
        std::string force_platform;
    };
}

void init_config();

int read_config_file(const char* path);

void show_config();

fs::path get_absolute_path(const char* path, fs::path work_dir);