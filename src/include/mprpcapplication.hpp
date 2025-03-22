#ifndef MPRPCAPP_H
#define MPRPCAPP_H

#include "config.hpp"
#include "logger.hpp"

// mprpc framework initialization
class MPRpcApp
{
public:
    static void Init(int argc, char *argv[]);
    static MPRpcApp &get_instance();
    static Config &get_config();

private:
    MPRpcApp();
    MPRpcApp(const MPRpcApp &) = delete;
    MPRpcApp(const MPRpcApp &&) = delete;
    MPRpcApp &operator=(const MPRpcApp &) = delete;

    static Config config;
};

#endif