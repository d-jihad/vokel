#include "engine.hpp"
#include <exception>
#include <iostream>
#include <stdlib.h>

int main()
{
    try {
        VoKel::Engine engine {};
        engine.run();

    } catch (const std::exception& exception) {
        std::cout << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}