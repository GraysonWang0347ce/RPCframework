#include"test.pb.h"
#include<iostream>

int main(int argc, char* atgv[])
{
    test::Login login;
    login.set_name("lingyuwenn");
    std::cout<<login.name();
}