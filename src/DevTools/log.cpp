// WHY MUST I DO THIS????????????????????????????????

#include "Engine/Log.hpp"
#include <exception>
#include <stdexcept>

void __log(std::string text, termcolors::color color)
{
#ifndef __EMSCRIPTEN__
    std::cout << termcolors::foreground_color(color) << text << std::endl << termcolors::reset;
#else
    std::cout << text << std::endl;
#endif
}

void __logFileLines(std::string prefix, std::string text, const char* file, int line, termcolors::color color)
{
#ifndef __EMSCRIPTEN__
    std::cout << termcolors::foreground_color(color) 
            << "At " << file << ":" << line << std::endl
                 << "\t" << prefix << text << std::endl
                 << termcolors::reset;
#else
    std::cout << "At " << file << ":" << line << std::endl
                 << "\t" << prefix << text << std::endl;
#endif
}

void __logAssert(bool condition, bool crash, std::string message, const char* file, int line)
{
    if (condition)
    {
        __logFileLines("Assert failed: ", message, file, line, termcolors::color::red);
        if (crash)
        {
            throw AssertFailed("Assert failed: " + message);
        }
    }
}