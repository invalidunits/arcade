#ifndef _ARCADE_ERR
#define _ARCADE_ERR

#include <string>
#include <stdexcept>
#include <SDL2/SDL_error.h>

void err(std::string_view error_message, ...);

struct sdl_exception: public std::exception {
    const char* what() const _NOEXCEPT {
        return SDL_GetError();
    }
};




#endif //#ifndef _ARCADE_ERR
