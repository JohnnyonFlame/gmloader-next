#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace gmloader {
    // a simple struct to model a person
    struct config {
        std::string save_dir;
        std::string apk_path;
        bool show_cursor;
    };
}

int read_config_file(const char* path);

void show_config();