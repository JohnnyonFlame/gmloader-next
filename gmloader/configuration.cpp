#include "configuration.h"
#include "platform.h"

gml_config gmloader_config = {};

#define get_if_exists(value, var) \
    if (j.contains(value)) j.at(value).get_to(c.var);
void from_json(const json& j, gml_config& c) {
    get_if_exists("save_dir", save_dir);
    get_if_exists("apk_path", apk_path);
    get_if_exists("show_cursor", show_cursor);
    get_if_exists("disable_controller", disable_controller);
    get_if_exists("disable_depth", disable_depth);
    get_if_exists("disable_extensions", disable_extensions);
    get_if_exists("disable_rumble", disable_rumble);
    get_if_exists("rumble_scale", rumble_scale);
    get_if_exists("disable_texhack", disable_texhack);
    get_if_exists("force_platform", force_platform);
}

void gml_config::init_defaults(){
    apk_path = "game.apk";
    save_dir = "";
    show_cursor = true;
    disable_controller = false;
    disable_depth = false;
    disable_extensions = true;
    disable_rumble = false;
    rumble_scale = 1.0;
    disable_texhack = true; /* Disabled by default until properly tested. */
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
    printf("config: save_dir = %s\n", save_dir.c_str());
    printf("config: apk_path = %s\n", apk_path.c_str());
    printf("config: show_cursor = %d\n", show_cursor);
    printf("config: disable_controller = %d\n", disable_controller);
    printf("config: disable_depth = %d\n", disable_depth);
    printf("config: disable_extensions = %d\n", disable_extensions);
    printf("config: disable_rumble = %d\n", disable_rumble);
    printf("config: rumble_scale = %f\n", rumble_scale);
    printf("config: disable_texhack = %d\n", disable_texhack);
    printf("config: force_platform = %s\n", force_platform.c_str());
}