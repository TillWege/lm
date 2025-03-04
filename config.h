#ifndef CONFIG_H
#define CONFIG_H
#include <string>

enum ConfigKey {
    PROVIDER,
    MODEL_NAME,
    TEMPERATURE,
};

void initConfigFile();
void loadConfig();
std::string getConfig(ConfigKey key);

#endif //CONFIG_H
