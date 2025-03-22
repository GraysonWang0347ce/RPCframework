#include "mprpccontroller.hpp"



RpcControl::RpcControl()
{
    _failed = false;
    _err_text = "";
}

void RpcControl::Reset()
{
    _failed = false;
    _err_text = "";

    return;
}

bool RpcControl::Failed() const
{
    return _failed;
}

std::string RpcControl::ErrorText() const
{
    return _err_text;
}

void RpcControl::SetFailed(const std::string &reason)
{
    _failed = true;
    _err_text = reason;
}