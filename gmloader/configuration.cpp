#include "configuration.h"
#include "platform.h"

gml_config gmloader_config = {};

void gml_config::init_defaults(){
    apk_path = "game.apk";
    save_dir = "";
    show_cursor = true;
    disable_controller = false;
    disable_depth = false;
    force_platform = "os_android";
}

int gml_config::parse_file(const char* path) {
    printf("Loading config file (%s)\n", path);

    std::ifstream config_file(path);
    if (!config_file.is_open()) {
        std::cerr << "Error: Could not open config file at " << path << "\n";
        return -1;
    }

    json config_json;
    try {
        config_file >> config_json;
        config_json.get_to(*this);
    } catch (const std::exception& e) {
        std::cerr << "Error applying configuration: " << e.what() << "\n";
        return -1;
    }

    return 1;
}

void gml_config::show_config(){
    printf("config: save_dir = %s\n",gmloader_config.save_dir.c_str());
    printf("config: apk_path = %s\n",gmloader_config.apk_path.c_str());
    printf("config: show_cursor = %d\n",gmloader_config.show_cursor);
    printf("config: disable_controller = %d\n",gmloader_config.disable_controller);
    printf("config: disable_depth = %d\n",gmloader_config.disable_depth);
    printf("config: force_platform = %s\n",gmloader_config.force_platform.c_str());
}