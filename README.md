# An implementation of RPC framework

## How to use

### publisher

1. In .proto , use service to describe your service and method, like below

```proto
option cc_generic_services=true;

message Request
{
    bytes params=1;
    bytes params=2;
}

message Response
{
   bytes params=1;
    bytes params=2;
}

service YourService
{
    rpc YourMethod(Request)returns(Response);
    rpc YourMethod2(Request)returns(Response);
}
```

2. Extend your class from namespace::YourService, override Service function generated in *.pb.cc, and call your local method

```c++
class Service : public namespace::YourService
{
public:
    bool LocalMethod()
    {
        // Local method here
    }

    virtual void YourMethod(::google::protobuf::RpcController *controller,
                       const ::rpc::LoginRequest *request,
                       ::rpc::LoginResponse *response,
                       ::google::protobuf::Closure *done) override
    {
       // Call LocalMethod here
    };
};
```

3. Use basic api to publish your service, enjoy!

```c++
int main(int argc, char **argv)
{
    MPRpcApp::Init(argc, argv);

    RpcProvider provider;
    UserService us;
    provider.notify_service(&us);

    provider.Run();

    return 0;
}
```

### caller

1. Initiate framework, new a channel and call remote service using stub

```c++
    MPRpcApp::Init(argc, argv);
    google::protobuf::RpcChannel *channel = new MprpcChannel();
    rpc::UserServiceRpc_Stub stub(channel);

    stub.Login(&ctl, &req, &res, nullptr);

    delete channel;
```
