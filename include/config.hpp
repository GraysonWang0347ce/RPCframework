#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <unordered_map>
#include <string>
#include <iostream>

/// @brief Get config files, keys are
///          rpcserverip rpcserverport zookeeperip zookeeperport
class Config
{
public:
    void LoadConfigFile(const std::string &file);

    std::string Load(const std::string &key);

private:
    std::unordered_map<std::string, std::string> config_map;
    std::string Trim(std::string &);
};

#endif