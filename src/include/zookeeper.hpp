#ifndef ZOOKEEPER_H
#define ZOOKEEPER_H

#include <zookeeper/zookeeper.h>
#include <string>
#include <semaphore.h>

class ZKclient
{
public:
    ZKclient();
    ~ZKclient();

    // @brief To start connecting ZK server
    void start();

    // @brief Create znode on connected ZK server
    // @param state 0 on permanent node, 1 on ephemeral one
    void create(const char *path,
                const char *data,
                int data_len,
                int state);

    std::string get_data(const char *path);

private:
    zhandle_t *_zk_handle;
};

#endif