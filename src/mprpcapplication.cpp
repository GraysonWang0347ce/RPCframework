#include "mprpcapplication.hpp"
#include <iostream>
#include <unistd.h>

Config MPRpcApp::config;

void MPRpcApp::Init(int argc, char *argv[])
{
    if (argc < 2)
    {
        // Error log
        std::cout << "Initialization error! check your input" << std::endl;
        LOG_ERROR("Initialization error! check your input ( %s : %s : %s )", __FILE__, __FUNCTION__, __LINE__);
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    while ((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            std::cout << "Invalid input!" << std::endl;
            LOG_ERROR("Invalid input! ( %s : %s : %s )", __FILE__, __FUNCTION__, __LINE__);
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // Load config files
    config.LoadConfigFile(config_file);

    std::cout << "rpcserverip: " << config.Load("rpcserverip") << std::endl;
    std::cout << "rpcserverport: " << config.Load("rpcserverport") << std::endl;
    std::cout << "zookeeperip: " << config.Load("zookeeperip") << std::endl;
    std::cout << "zookeeperport: " << config.Load("zookeeperport") << std::endl;
    LOG_INFO("rpcserverip: %s\nrpcserverport: %s\nzookeeperip: %s\nzookeeperport: %s",
             config.Load("rpcserverip").c_str(), config.Load("rpcserverport").c_str(),
             config.Load("zookeeperip").c_str(), config.Load("zookeeperport").c_str());
}

MPRpcApp &MPRpcApp::get_instance()
{
    static MPRpcApp app;
    return app;
}
Config &MPRpcApp::get_config()
{
    return config;
};

MPRpcApp::MPRpcApp()
{
}