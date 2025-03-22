#include "rpcprovider.hpp"
#include "mprpcapplication.hpp"
#include "./include/rpcheader.pb.h"
#include "../src/include/zookeeper.hpp"

void RpcProvider::notify_service(google::protobuf::Service *service)
{
    ServiceInfo service_info;

    const google::protobuf::ServiceDescriptor *service_desc = service->GetDescriptor();

    // get metadata of service
    std::string service_name = service_desc->name();
    int method_cnt = service_desc->method_count();

    std::cout << "Service name: " << service_name << std::endl;

    for (int i = 0; i < method_cnt; ++i)
    {
        const google::protobuf::MethodDescriptor *method_desc = service_desc->method(i);
        std::string method_name = method_desc->name();
        service_info.name2method.insert({method_name, method_desc});

        std::cout << "Method name: " << method_name << std::endl;
    }
    service_info.service = service;
    service_map.insert({service_name, service_info});
}

void RpcProvider::Run()
{
    std::string ip = MPRpcApp::get_instance().get_config().Load("rpcserverip");
    uint16_t port = atoi(MPRpcApp::get_instance().get_config().Load("rpcserverport").c_str());
    muduo::net::InetAddress inet(ip, port);

    muduo::net::TcpServer tcp_server_ptr(&loop, inet, "RpcServer");

    tcp_server_ptr.setThreadNum(8);

    ZKclient zkClient;
    zkClient.start();
    for (auto &sp : service_map)
    {
        std::string service_path = "/" + sp.first;
        zkClient.create(service_path.c_str(), nullptr, 0,0);
        for (auto &mp : sp.second.name2method)
        {
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            zkClient.create(method_path.c_str(),
                            method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    tcp_server_ptr.setConnectionCallback(std::bind(&RpcProvider::on_conn, this, std::placeholders::_1));

    tcp_server_ptr.setMessageCallback(std::bind(&RpcProvider::on_msg, this,
                                                std::placeholders::_1,
                                                std::placeholders::_2,
                                                std::placeholders::_3));

    std::cout << "Rpc Server statred at ip:" << ip << ", port:" << port << std::endl;

    tcp_server_ptr.start();
    loop.loop();
}

void RpcProvider::on_conn(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        // Connection broke
        conn->shutdown();
    }
}

void RpcProvider::on_msg(const muduo::net::TcpConnectionPtr &conn,
                         muduo::net::Buffer *buffer,
                         muduo::Timestamp time)
{
    std::string recv_buf = buffer->retrieveAllAsString();

    // Read first 4 bytes as header size
    uint32_t header_size;
    recv_buf.copy((char *)&header_size, 4, 0);

    std::cout << "Header Size: " << header_size << std::endl;

    // Read raw byte_stream accorcding to header size
    std::string rpc_header_str = recv_buf.substr(4, header_size);

    // Nonserialize
    rpcheader::RpcHeader rpcheader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    if (rpcheader.ParseFromString(rpc_header_str))
    {
        service_name = rpcheader.service_name();
        method_name = rpcheader.meghod_name();
        args_size = rpcheader.args_size();
    }
    else
    {
        std::cout << "Syntax error in Rpc Header, got " << rpc_header_str << std::endl;
        return;
    }

    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    std::cout << "Header size: " << header_size << std::endl;
    std::cout << "rpc header string: " << rpc_header_str << std::endl;
    std::cout << "service name: " << service_name << std::endl;
    std::cout << "method name: " << method_name << std::endl;

    // Get service & method object
    auto it = service_map.find(service_name);
    if (it == service_map.end())
    {
        std::cout << "Service name not applicable! got " << service_name << std::endl;
        return;
    }

    auto it_m = it->second.name2method.find(method_name);
    if (it_m == it->second.name2method.end())
    {
        std::cout << "Method name not applicable! got " << method_name << std::endl;
        return;
    }

    google::protobuf::Service *service = it->second.service;
    const google::protobuf::MethodDescriptor *method = it_m->second;

    // Generate request & response's parammeters
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))
    {
        std::cout << "Request parsing failed! got " << args_str << std::endl;
        return;
    }

    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // Create closure
    google::protobuf::Closure *done =
        google::protobuf::NewCallback<RpcProvider,
                                      const muduo::net::TcpConnectionPtr &,
                                      google::protobuf::Message *>(this, &RpcProvider::send_rpc_response, conn, response);

    // Call local method according to remote call protocol
    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::send_rpc_response(const muduo::net::TcpConnectionPtr &conn,
                                    google::protobuf::Message *msg)
{
    std::string serialized_string;
    msg->SerializeToString(&serialized_string);

    conn->send(serialized_string);

    // Simulates http short link
    conn->shutdown();
}
