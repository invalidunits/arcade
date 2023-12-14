#ifndef _ARCADE_GRAPHICS
#define _ARCADE_GRAPHICS

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>
#include <system/math.hxx>
#include <string>
#include <system/err.hxx>
#include <memory>
#include <rom/rom.hxx>


// Define window properties
#define ARCADE_LOGIC_WIDTH  (224)
#define ARCADE_LOGIC_HEIGHT (288)


#define ARCADE_WINDOW_WIDTH  (ARCADE_LOGIC_WIDTH*3)
#define ARCADE_WINDOW_HEIGHT (ARCADE_LOGIC_HEIGHT*3)
#define ARCADE_WINDOW_PROPERTIES (0)

// Define SDL wrappers.
namespace Graphics {
    inline SDL_Window *window = nullptr;
    inline SDL_Renderer *renderer = nullptr;

    
    inline TTF_Font *default_font = nullptr;

    using shared_texture = std::shared_ptr<SDL_Texture>;
    using shared_surface = std::shared_ptr<SDL_Surface>;

    shared_texture loadTexture(void *data, std::size_t size);
    shared_surface loadSurface(void *data, std::size_t size);


    
    #define ARDCADE_LOADSURFROM(rom) ::Graphics::loadSurface((void *)ROM::g ## rom ## Data, ROM::g ## rom ## Size) 
    #define ARDCADE_LOADTEXTROM(rom) ::Graphics::loadTexture((void *)ROM::g ## rom ## Data, ROM::g ## rom ## Size) 

    void drawText(Math::recti bounds, std::string_view text, SDL_Renderer *renderer,
        Math::color8a color = Math::color8a(~0, ~0, ~0, ~0), TTF_Font *font = nullptr);
}





#endif //#ifndef _ARCADE_GRAPHICS