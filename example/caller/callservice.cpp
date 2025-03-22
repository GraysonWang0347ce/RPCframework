#include <iostream>
#include "mprpcapplication.hpp"
#include "../user.pb.h"
#include "../src/include/mprpcchannel.hpp"
#include "../src/include/mprpccontroller.hpp"

int main(int argc, char **argv)
{
    LOG_INFO("Caller Initiated!");
    LOG_ERROR("%s:%s: %d",__FILE__,__FUNCTION__,__LINE__);

    MPRpcApp::Init(argc, argv);
    google::protobuf::RpcChannel *channel = new MprpcChannel();
    rpc::UserServiceRpc_Stub stub(channel);
    RpcControl ctl;

    rpc::LoginRequest req;
    std::string *name = req.mutable_name();
    std::string *psw = req.mutable_pwd();

    *name = "Foo";
    *psw = "Bar";

    rpc::LoginResponse res;
    stub.Login(&ctl, &req, &res, nullptr);

    delete channel;

    if (ctl.Failed())
    {
        std::cout << ctl.ErrorText() << std::endl;
        return -1;
    }

    if (res.result().errcode() == 200)
    {
        std::cout << "rpc login sussess! response: " << res.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login failure! response: " << res.result().errmsg() << std::endl;
    }
    return 0;
}