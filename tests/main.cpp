#include <iostream>

#include <window.hpp>

int main()
{
    std::cout << "Window test\n";

    tmig::Window w{"window test"};
    w.start();

    return 0;
}