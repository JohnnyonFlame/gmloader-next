#include <filesystem>
#include <string>
#include <fstream>
#include <iostream>
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

        // JSON deserialization
        friend void from_json(const json& j, config& c) {
            j.at("save_dir").get_to(c.save_dir);
            j.at("apk_path").get_to(c.apk_path);
            j.at("show_cursor").get_to(c.show_cursor);
            j.at("disable_controller").get_to(c.disable_controller);
            j.at("disable_depth").get_to(c.disable_depth);
            j.at("force_platform").get_to(c.force_platform);
        }
    };
}

void init_config();

int read_config_file(const char* path);

void show_config();

fs::path get_absolute_path(const char* path, fs::path work_dir);

extern gmloader::config gmloader_config;