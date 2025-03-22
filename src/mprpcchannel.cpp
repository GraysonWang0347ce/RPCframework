#include "mprpcchannel.hpp"
#include "../src/include/rpcheader.pb.h"
#include "../src/include/mprpcapplication.hpp"
#include "mprpccontroller.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "zookeeper.hpp"

void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller,
                              const google::protobuf::Message *request,
                              google::protobuf::Message *response,
                              google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();

    std::string argc_str;
    if (!request->SerializeToString(&argc_str))
    {
        std::cout << "Serialize request error! " << std::endl;
        return;
    }

    int argc_size = argc_str.size();

    // Rpc request header
    rpcheader::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_meghod_name(method_name);
    rpc_header.set_args_size(argc_size);

    std::string rpc_header_str;
    if (!rpc_header.SerializeToString(&rpc_header_str))
    {
        std::cout << "Request header serialize error! " << std::endl;
        controller->SetFailed("Request header serialize error!");
        return;
    }

    uint32_t header_size = rpc_header_str.size();
    std::cout << "Header size: " << header_size << std::endl;

    // Send
    std::string send_str;
    send_str.insert(0, std::string((char *)&header_size, 4));
    send_str += rpc_header_str;
    send_str += argc_str;

    std::cout << "Send str: " << send_str << std::endl;

    // Create socket to send request
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        std::cout << "Create socket failure! errno: " << errno << std::endl;
        char txt[512];
        sprintf(txt, "Create socket failure! errno: %d", errno);
        controller->SetFailed(txt);
        return;
    }

    // Get rpc server socket
    // std::string ip = MPRpcApp::get_instance().get_config().Load("rpcserverip");
    // uint16_t port = atoi(MPRpcApp::get_instance().get_config().Load("rpcserverport").c_str());
    ZKclient zkcli;
    zkcli.start();

    std::string method_path = "/" + service_name + "/" + method_name;
    std::string host_data = zkcli.get_data(method_path.c_str());

    if (host_data == "")
    {
        controller->SetFailed(method_path + " not exists. ");
        return;
    }

    int idx = host_data.find(':');

    std::string ip = host_data.substr(0,idx);
    uint16_t port = atoi(host_data.substr(idx+1,host_data.size()-idx).c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (-1 == connect(client_fd, (sockaddr *)&server_addr, sizeof(server_addr)))
    {
        std::cout << "Connect failure! errno: " << errno << std::endl;
        close(client_fd);
        char txt[512];
        sprintf(txt, "Connect failure! errno:  %d", errno);
        controller->SetFailed(txt);

        return;
    }

    if (-1 == send(client_fd, send_str.c_str(), send_str.size(), 0))
    {
        std::cerr << "Send Msg failure! errno: " << errno << std::endl;
        char txt[512];
        sprintf(txt, "Send Msg failure! errno: %d", errno);
        controller->SetFailed(txt);
        return;
    }

    // Receive rpc response
    char buffer[1024] = {0};
    int recv_size;
    if (-1 == (recv_size = recv(client_fd, buffer, 1024, 0)))
    {
        std::cerr << "Receive failure! errno: " << errno << std::endl;
        close(client_fd);
        char txt[512];
        sprintf(txt, "Receive failure! errno:  %d", errno);
        controller->SetFailed(txt);
        return;
    }

    // Deserializing
    // std::string response_str(buffer, 0, recv_size);
    // if (!response->ParseFromString(response_str))
    if (!response->ParseFromArray(buffer, recv_size))
    {
        std::cerr << "Parsing from remote failure! " << std::endl;
        close(client_fd);
        controller->SetFailed("Parsing from remote failure!");
        return;
    }

    close(client_fd);
}
