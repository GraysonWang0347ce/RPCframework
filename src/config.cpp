#include "config.hpp"

void Config::LoadConfigFile(const std::string &file)
{
    FILE *f = fopen(file.c_str(), "r");
    if (f == nullptr)
    {
        std::cerr << "Configfile path not exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (!feof(f))
    {
        /*
            1. notations
            2. specific content
            3. \t,\n
        */
        char buf[542];
        fgets(buf, 542, f);

        std::string buffer(buf);
        buffer = Trim(buffer);

        if (buffer == "")
            continue;

        // real config file
        int idx = buffer.find('=');
        if (idx == -1)
            continue; // illeagal config syntax

        std::string key;
        std::string value;

        key = buffer.substr(0, idx);
        value = buffer.substr(idx + 1, buffer.size() - idx + 1);

        Trim(key);

        value = value.substr(0, value.size() - 1);

        config_map.insert({key, value});
    }
}

std::string Config::Load(const std::string &key)
{
    auto it = config_map.find(key);
    return it == config_map.end() ? std::string() : it->second;
}

/// @brief Trim spaces, tabs and hashtags up front and behind
/// @param buffer string to be trimed
/// @return clean string or empty string if invalid
std::string Config::Trim(std::string &buffer)
{
    // cancel tabs
    int idx = buffer.find_first_not_of(' ');
    if (idx != -1)
        buffer = buffer.substr(idx, buffer.size() - idx);

    // cancel spacings at the back
    idx = buffer.find_last_not_of(' ');
    if (idx != -1)
        buffer = buffer.substr(0, idx + 1);

    // annotations
    if (buffer[0] == '#' || buffer.empty())
        return "";

    return buffer;
}