#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <cstdlib>

extern std::string logo;

#ifdef _WIN32
    #define strcasecmp _stricmp
    #include <windows.h>

    inline void enableVTMode() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }

#else
    #include <strings.h>
    inline void enableVTMode() {
       return;
    }

#endif

#endif // UTILS_H
