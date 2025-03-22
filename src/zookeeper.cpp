#include "zookeeper.hpp"
#include "mprpcapplication.hpp"
#include <iostream>

void global_watcher(zhandle_t *zh, int type,
                    int state, const char *path, void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT)
    {
        if (state == ZOO_CONNECTED_STATE)
        {
            sem_t *sem = (sem_t *)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZKclient::ZKclient() : _zk_handle(nullptr)
{
}

ZKclient::~ZKclient()
{
    if (_zk_handle != nullptr)
        zookeeper_close(_zk_handle);
}

void ZKclient::start()
{
    std::string host = MPRpcApp::get_instance().get_config().Load("zookeeperip");
    std::string port = MPRpcApp::get_instance().get_config().Load("zookeeperport");
    std::string conn_str = host + ":" + port;

    _zk_handle = zookeeper_init(conn_str.c_str(), global_watcher, 30000,
                                nullptr, nullptr, 0);

    if (_zk_handle == nullptr)
    {
        LOG_ERROR("zookeeper Initiation error! ");
        return;
    }

    sem_t sem;
    sem_init(&sem, 0, 0);

    zoo_set_context(_zk_handle, &sem);
    sem_wait(&sem);

    LOG_INFO("zookeeper initiating success!");
    return;
}

void ZKclient::create(const char *path,
                      const char *data,
                      int data_len,
                      int state = 0)
{
    char path_buffer[128];
    int buff_len = sizeof(path_buffer);
    int flag;
    flag = zoo_exists(_zk_handle, path, 0, nullptr);

    if (flag == ZNONODE)
    {
        flag = zoo_create(_zk_handle, path, data, data_len,
                          &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, buff_len);

        if (flag == ZOK)
        {
            LOG_INFO("Znode Create Success... path: %s", path);
        }
        else
        {
            LOG_ERROR("Znode create failure... path: %s,status: %d", path, flag);
            return;
        }
    }
}

std::string ZKclient::get_data(const char *path)
{
    char buffer[1024];
    int buffer_len = sizeof(buffer);
    int flag = zoo_get(_zk_handle, path, 0, buffer, &buffer_len, nullptr);
    if (flag == ZOK)
        return buffer;
    else
    {
        LOG_ERROR("Get data from %s error! status: %d", path, flag);
        return "";
    }
}