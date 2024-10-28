#include "configuration.h"
#include "platform.h"

gmloader::config gmloader_config;

void init_config(){
    // default values
    gmloader_config.apk_path = "game.apk";
    gmloader_config.save_dir = "";
    gmloader_config.show_cursor = true;
}

int read_config_file(const char* path){
    printf("Loading config file (%s)\n", path);

    std::ifstream config_file(path);
    json config_json;

    int p_loaded = 0;

    try{
        config_json = json::parse(config_file);
    }catch (const json::parse_error& e){
        warning("\tparse error (%s)\n", e.what());
        return -1;
    } 

    try{
        config_json.at("save_dir").get_to(gmloader_config.save_dir);
        p_loaded++;
    }catch (const json::out_of_range& e){
        // default value
    }catch (const json::type_error& e){
        warning("\tsave_dir type error\n");
    }
    printf("\tsave_dir = %s\n",gmloader_config.save_dir.c_str());
    
    try{
        config_json.at("apk_path").get_to(gmloader_config.apk_path);
        p_loaded++;
    }catch (const json::out_of_range& e){
        // default value
    }catch (const json::type_error& e){
        warning("\tapk_path type error\n");
    }
    printf("\tapk_path = %s\n",gmloader_config.apk_path.c_str());

    try{
        config_json.at("show_cursor").get_to(gmloader_config.show_cursor);
        p_loaded++;
    }catch (const json::out_of_range& e){
        // default value
    }catch (const json::type_error& e){
        warning("\tshow_cursor type error\n");
    }
    printf("\tshow_cursor = %d\n",gmloader_config.show_cursor);

    return p_loaded;
}

void show_config(){
    printf("config: save_dir = %s\n",gmloader_config.save_dir.c_str());
    printf("config: apk_path = %s\n",gmloader_config.apk_path.c_str());
    printf("config: show_cursor = %d\n",gmloader_config.show_cursor);
}

fs::path get_absolute_path(const char* path, fs::path work_dir){

    fs::path fs_path = fs::path(path);
    
    if ( fs_path.is_relative() ){
        fs_path = work_dir / fs_path;
    }

    return fs_path;
}