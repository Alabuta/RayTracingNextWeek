#pragma once

#include <iostream>
#include <sstream>
#include <array>
#include <vector>
#include <chrono>

#ifdef _MSC_VER
    #include <execution>
#endif

#include <string>
using namespace std::string_literals;

#include <string_view>
using namespace std::string_view_literals;

#include "config.hxx"

#include "math/math.hxx"



template<typename T = std::chrono::milliseconds>
struct measure {
    template<typename F, typename... Args>
    static auto execution(F func, Args &&... args)
    {
        auto start = std::chrono::system_clock::now();

        func(std::forward<Args>(args)...);

        auto duration = std::chrono::duration_cast<T>(std::chrono::system_clock::now() - start);

        return duration.count();
    }
};


#ifdef _DEBUG
    #ifdef _MSC_VER
        #define _CRTDBG_MAP_ALLOC
        #include <crtdbg.h>
    #else
        #include <csignal>

        void posix_signal_handler(int signum)
        {
            auto currentThread = std::this_thread::get_id();

            auto name = "unknown"s;

            switch (signum) {
                case SIGABRT: name = "SIGABRT"s;  break;
                case SIGSEGV: name = "SIGSEGV"s;  break;
                case SIGBUS:  name = "SIGBUS"s;   break;
                case SIGILL:  name = "SIGILL"s;   break;
                case SIGFPE:  name = "SIGFPE"s;   break;
                case SIGTRAP: name = "SIGTRAP"s;  break;
            }

            std::array<void *, 32> callStack;

            auto size = backtrace(std::data(callStack), std::size(callStack));

            std::cerr << "Error: signal "s << name << std::endl;

            auto symbollist = backtrace_symbols(std::data(callStack), size);

            for (auto i = 0; i < size; ++i)
                std::cerr << i << ' ' << currentThread << ' ' << symbollist[i] << '\n';

            free(symbollist);

            exit(1);
        }
    #endif
#endif
