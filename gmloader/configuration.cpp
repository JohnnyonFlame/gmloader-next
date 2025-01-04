#include "configuration.h"
#include "platform.h"

gmloader::config gmloader_config;

void init_config(){
    // default values
    gmloader_config.apk_path = "game.apk";
    gmloader_config.save_dir = "";
    gmloader_config.show_cursor = true;
    gmloader_config.disable_controller = false;
    gmloader_config.disable_depth = false;
    gmloader_config.force_platform = "os_android";
}

int read_config_file(const char* path) {
    printf("Loading config file (%s)\n", path);

    std::ifstream config_file(path);
    if (!config_file.is_open()) {
        std::cerr << "Error: Could not open config file at " << path << "\n";
        return -1;
    }

    json config_json;
    try {
        config_file >> config_json;
    } catch (const json::parse_error& e) {
        std::cerr << "Parse error: " << e.what() << "\n";
        return -1;
    }

    try {
        config_json.get_to(gmloader_config);
    } catch (const std::exception& e) {
        std::cerr << "Error applying configuration: " << e.what() << "\n";
        return -1;
    }

    return 1;
}

void show_config(){
    printf("config: save_dir = %s\n",gmloader_config.save_dir.c_str());
    printf("config: apk_path = %s\n",gmloader_config.apk_path.c_str());
    printf("config: show_cursor = %d\n",gmloader_config.show_cursor);
    printf("config: disable_controller = %d\n",gmloader_config.disable_controller);
    printf("config: disable_depth = %d\n",gmloader_config.disable_depth);
    printf("config: force_platform = %s\n",gmloader_config.force_platform.c_str());
}

fs::path get_absolute_path(const char* path, fs::path work_dir){

    fs::path fs_path = fs::path(path);
    
    if ( fs_path.is_relative() ){
        fs_path = work_dir / fs_path;
    }

    return fs_path;
}