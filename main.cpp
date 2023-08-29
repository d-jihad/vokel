#include "app.hpp"
#include <exception>
#include <iostream>
#include <stdlib.h>

int main()
{
    try {
        App app { 900, 700 };
        app.run();

    } catch (const std::exception& exception) {
        std::cout << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}