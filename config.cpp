#include "config.h"
#include <fstream>
#include <iostream>
#include "json.hpp"
#include "cfgpath.h"

nlohmann::json config;

std::string getConfigPath() {
    char cfgdir[512];
    get_user_config_file(cfgdir, sizeof(cfgdir), "lm");
    if (cfgdir[0] == 0) {
        printf("Unable to find home directory.\n");
    }
    std::string path(cfgdir);
    return path;
}

void initConfigFile() {
    std::string path = getConfigPath();
    nlohmann::json defaultConfig;
    defaultConfig["model"] = "qwen2.5-coder:1.5b";
    defaultConfig["temperature"] = 1.0;
    defaultConfig["provider"] = "ollama";

    std::ofstream configFile(path);
    if (!configFile.is_open()) {
        std::cerr << "Error: Could not open config file at " << path << std::endl;
        return;
    }

    configFile << defaultConfig.dump(4) << std::endl;
    configFile.flush();
    configFile.close();
}

void loadConfig() {

    std::string path = getConfigPath();

    if(!std::filesystem::exists(path)) {
        initConfigFile();
    }

    std::ifstream configFile(path);
    if (configFile.is_open()) {
        configFile >> config;
        configFile.close();
        //std::cout << "Config loaded" << std::endl;
        //std::cout << config.dump(4) << std::endl;
    }
}

std::string getConfig(ConfigKey key)
{
    switch (key) {
        case ConfigKey::PROVIDER:
            return config["provider"];
        case ConfigKey::MODEL_NAME:
            return config["model"];
        case ConfigKey::TEMPERATURE:
            int t = config["temperature"];
            return std::to_string(t);
    }
    return "";
}
