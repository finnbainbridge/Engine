/*
Logging and assertions for Engine

*/

#ifndef ENGINE_LOG_H
#define ENGINE_LOG_H

#include <iostream>
#include <string>

#include <term-colors/termcolors.h>

// No fancy classes for this one

// Apart from this exception
class AssertFailed: public std::exception
{
    private:
        std::string message;
    public:
        AssertFailed(std::string msg): std::exception()
        {
            message = msg;
        }
        const char* what() const throw() { return message.c_str(); }
};


void __log(std::string text, termcolors::color color);

void __logFileLines(std::string prefix, std::string text, const char* file, int line, termcolors::color color);


#define LOG_SUCCESS(txt) __log(txt, termcolors::color::green)
#define LOG_INFO(txt) __log(txt, termcolors::color::blue)
#define LOG_WARN(txt) __logFileLines("WARNING: ", txt, __FILE__, __LINE__, termcolors::color::yellow)
#define LOG_ERROR(txt) __logFileLines("ERROR: " , txt, __FILE__, __LINE__, termcolors::color::red)

void __logAssert(bool condition, bool crash, std::string message, const char* file, int line);

#define LOG_ASSERT_MESSAGE(cond, txt) __logAssert(cond, false, txt, __FILE__, __LINE__)
#define LOG_ASSERT(cond) __logAssert(cond, false, "Value != true", __FILE__, __LINE__)

#define LOG_ASSERT_MESSAGE_FATAL(cond, txt) __logAssert(cond, true, txt, __FILE__, __LINE__)
#define LOG_ASSERT_FATAL(cond) __logAssert(cond, true, "Value != true", __FILE__, __LINE__)


#endif