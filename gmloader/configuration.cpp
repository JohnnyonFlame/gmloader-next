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
    json config_json;

    int p_loaded = 0;

    try {
        config_json = json::parse(config_file);
    } catch (const json::parse_error& e) {
        warning("\tparse error (%s)\n", e.what());
        return -1;
    }
    
    gmloader_config.save_dir = config_json.value("save_dir", std::string("default_save_dir"));
    printf("\tsave_dir = %s\n", gmloader_config.save_dir.c_str());
    p_loaded += !gmloader_config.save_dir.empty();

    gmloader_config.apk_path = config_json.value("apk_path", std::string("default_apk_path"));
    printf("\tapk_path = %s\n", gmloader_config.apk_path.c_str());
    p_loaded += !gmloader_config.apk_path.empty();

    gmloader_config.show_cursor = config_json.value("show_cursor", false);
    printf("\tshow_cursor = %d\n", gmloader_config.show_cursor);
    p_loaded++;

    gmloader_config.disable_controller = config_json.value("disable_controller", false);
    printf("\tdisable_controller = %d\n", gmloader_config.disable_controller);
    p_loaded++;

    gmloader_config.disable_depth = config_json.value("disable_depth", false);
    printf("\tdisable_depth = %d\n", gmloader_config.disable_depth);
    p_loaded++;

    gmloader_config.force_platform = config_json.value("force_platform", std::string("default_platform"));
    printf("\tforce_platform = %s\n", gmloader_config.force_platform.c_str());
    p_loaded += !gmloader_config.force_platform.empty();

    return p_loaded;
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