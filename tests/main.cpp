#include <iostream>

#include "tmig/init.hpp"
#include "tmig/window.hpp"

int main()
{
    // Initialize tmig engine
    tmig::init();

    // Create window
    tmig::Window w{"window test"};

    // Run
    std::cout << "Window test\n";
    w.start();

    // Terminate tmig engine
    tmig::terminate();

    return 0;
}