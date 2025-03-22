#ifndef MPRPCCONTROLLER_H
#define MPRPCCONTROLLER_H

#include <google/protobuf/service.h>
#include <google/protobuf/message.h>
#include <string>

class RpcControl : public google::protobuf::RpcController
{
public:
    RpcControl();
    void Reset() override;
    bool Failed() const override;
    std::string ErrorText() const override;
    void SetFailed(const std::string &reason) override;

    // Unimplemented
    void NotifyOnCancel(google::protobuf::Closure *callback) {};
    bool IsCanceled() const { return false; };
    void StartCancel() {};

private:
    bool _failed;
    std::string _err_text;
};

#endif