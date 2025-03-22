#ifndef RPCPROVIDER_H
#define RPCPROVIDER_H

#include <google/protobuf/service.h>
#include <memory>
#include <muduo/net/TcpServer.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <functional>
#include <string>
#include <google/protobuf/descriptor.h>

class RpcProvider
{
public:
    /*
        @brief To publish local service into rpc service
        @param[in] service Concrete class extending Service base
    */
    void notify_service(google::protobuf::Service *);

    /*
        @brief Starting local rpc service, waiting to be called
    */
    void Run();

private:
    muduo::net::EventLoop loop;

    struct ServiceInfo
    {
        google::protobuf::Service *service;
        std::string name;
        std::unordered_map<std::string,
                           const google::protobuf::MethodDescriptor *>
            name2method; // Name -> Service Methods
    };

    std::unordered_map<std::string, ServiceInfo> service_map;

    void on_conn(const muduo::net::TcpConnectionPtr &);
    void on_msg(const muduo::net::TcpConnectionPtr &,
                muduo::net::Buffer *,
                muduo::Timestamp);

    /// @brief Closure method to send serialized stirng back
    void send_rpc_response(const muduo::net::TcpConnectionPtr &,
                           google::protobuf::Message *);
};
#endif