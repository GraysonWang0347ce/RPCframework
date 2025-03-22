#include <iostream>
#include "../user.pb.h"
#include "mprpcapplication.hpp"
#include "rpcprovider.hpp"

/*
    Local Service here
*/

class UserService : public rpc::UserServiceRpc
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "Login Success";
        return true;
    }

    virtual void Login(::google::protobuf::RpcController *controller,
                       const ::rpc::LoginRequest *request,
                       ::rpc::LoginResponse *response,
                       ::google::protobuf::Closure *done) override
    {
        std::string name = request->name();
        std::string psw = request->pwd();

        bool login_result = Login(name, psw);

        response->set_success(login_result);
        rpc::ResultCode *code = response->mutable_result();
        code->set_errcode(200);
        code->set_errmsg("Success!");

        done->Run();
    };
};

int main(int argc, char **argv)
{
    MPRpcApp::Init(argc, argv);

    RpcProvider provider;
    UserService us;
    provider.notify_service(&us);

    provider.Run();

    return 0;
}