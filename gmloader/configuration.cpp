#include "configuration.h"

#include "platform.h"

gmloader::config gmloader_config;

int read_config_file(const char* path){
    std::ifstream config_file(path);
    json config_json;

    int p_loaded = 0;

    // default values
    gmloader_config.apk_path = "game.apk";
    gmloader_config.save_dir = "gamedata";
    gmloader_config.show_cursor = true;

    try{
        config_json = json::parse(config_file);
    }catch (const json::parse_error& e){
        warning("%s: parse error (%s)\n", path, e.what());
        return -1;
    } 


    try{
        config_json.at("save_dir").get_to(gmloader_config.save_dir);
        p_loaded++;
    }catch (const json::out_of_range& e){
        printf("%s: save_dir default value\n", path);
    }catch (const json::type_error& e){
        warning("%s: save_dir type error\n", path);
    }
    
    try{
        config_json.at("apk_path").get_to(gmloader_config.apk_path);
        p_loaded++;
    }catch (const json::out_of_range& e){
        printf("%s: apk_path default value\n", path);
    }catch (const json::type_error& e){
        warning("%s: apk_path type error\n", path);
    }



    try{
        config_json.at("show_cursor").get_to(gmloader_config.show_cursor);
        p_loaded++;
    }catch (const json::out_of_range& e){
        printf("%s: show_cursor default value\n", path);
    }catch (const json::type_error& e){
        warning("%s: show_cursor type error\n", path);
    }

    return p_loaded;
}

void show_config(){
    printf("save_dir=%s\n",gmloader_config.save_dir.c_str());
    printf("apk_path=%s\n",gmloader_config.apk_path.c_str());
    printf("show_cursor=%d\n",gmloader_config.show_cursor);
}