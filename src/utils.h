#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <string.h>

#ifdef _WIN32
    #define strcasecmp _stricmp

    inline void clear_screen() {
        system("cls");
    }
#else
    #include <strings.h>

    inline void clear_screen() {
        system("clear");
    }
#endif

#endif // UTILS_H
