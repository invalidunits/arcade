
#define STB_IMAGE_IMPLEMENTATION
#include <extern/stb_image.h>

#include <graphics/graphics.hxx>
#include <cmath>
#include <stdexcept>
#include <map>
#include <system/err.hxx>
#include <cstring>
struct sdl_deleter
{
  void operator()(SDL_Window *p) const { SDL_DestroyWindow(p); }
  void operator()(SDL_Renderer *p) const { SDL_DestroyRenderer(p); }
  void operator()(SDL_Texture *p) const { SDL_DestroyTexture(p); }
  void operator()(SDL_Surface *p) const { SDL_FreeSurface(p); }
};


namespace Graphics {
    void drawText(Math::recti bounds, std::string_view text, SDL_Renderer *renderer,
        Math::color8a color, TTF_Font *font) {
        int err = 0;
        if (font == nullptr) {
            font = default_font;
        }

        if (std::min(bounds.w, bounds.h) == 0) {
            err = TTF_SizeText(font, text.data(), &bounds.w, &bounds.h);
            if (err) throw sdl_exception();
        }

        SDL_Surface *surface = TTF_RenderText_Solid_Wrapped(font, text.data(), 
            SDL_Color{color.r, color.b, color.g, color.a}, bounds.w);
        if (surface == nullptr) throw sdl_exception();

        auto texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture == nullptr) throw sdl_exception();
        
        auto rect = SDL_Rect{bounds.x, bounds.y, bounds.w, bounds.h};
        if (SDL_RenderCopy(renderer, texture, nullptr, &rect)) throw sdl_exception();

        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }

    void load_img() {}
    shared_surface loadSurface(void *data, std::size_t size) {
        Math::pointi dimentions;
        int depth = 0;
        auto rgba = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data), (int)size, &dimentions.w, &dimentions.h, &depth, STBI_rgb_alpha);
        if (rgba == nullptr) throw std::invalid_argument("data");
        auto surface = SDL_CreateRGBSurfaceWithFormatFrom(rgba, dimentions.w, dimentions.h, 32, 4*dimentions.w, SDL_PIXELFORMAT_RGBA32);
        stbi_image_free(rgba);
        if (!surface) throw sdl_exception();
        return std::shared_ptr<SDL_Surface>(surface, sdl_deleter());
    }

    shared_texture loadTexture(void *data, std::size_t size) {
        Math::pointi dimentions;
        int depth = 0;
        auto rgba = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data), (int)size, &dimentions.w, &dimentions.h, &depth, STBI_rgb_alpha);
        if (rgba == nullptr) throw std::invalid_argument("data");


        int pitch = 0;
        auto texture = SDL_CreateTexture(Graphics::renderer, SDL_PIXELFORMAT_RGBA32, 
                    SDL_TEXTUREACCESS_STREAMING,
                    dimentions.x, dimentions.y);        
        if (!texture) throw sdl_exception();
        void *target = nullptr;
        if (SDL_LockTexture(texture, nullptr, &target, &pitch)) throw sdl_exception();
        std::memcpy((void*)target, rgba, dimentions.h*dimentions.w*depth);
        stbi_image_free(rgba);
        SDL_UnlockTexture(texture);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
        return std::shared_ptr<SDL_Texture>(texture, sdl_deleter());
    }


}