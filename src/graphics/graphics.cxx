
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

    // cache
    std::map<std::pair<void *, std::size_t>, shared_texture::weak_type> texture_cache;
    std::map<std::pair<void *, std::size_t>, shared_surface::weak_type> surface_cache; 

    shared_surface loadSurface(void *data, std::size_t size, const char *debug_identidy) {
        if (debug_identidy)
            printf("Attempting to load %s...\n", debug_identidy);

        if (surface_cache.find({data, size}) != surface_cache.end()) {
            if (auto ret = surface_cache[{data, size}].lock())
                return ret;
        }


        Math::pointi dimentions;
        int depth = 0;
        auto rgba = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(data), (int)size, &dimentions.w, &dimentions.h, &depth, STBI_rgb_alpha);
        if (rgba == nullptr) throw std::invalid_argument("data");
        auto surface = SDL_CreateRGBSurfaceWithFormat(0, dimentions.w, dimentions.h, 32, SDL_PIXELFORMAT_RGBA32);
        if (!surface) throw sdl_exception();
        SDL_LockSurface(surface);
        std::memcpy(surface->pixels, rgba, dimentions.w*dimentions.h*4);
        SDL_UnlockSurface(surface);


        stbi_image_free(rgba);
        auto ret = std::shared_ptr<SDL_Surface>(surface, sdl_deleter());
        surface_cache.insert_or_assign({data, size}, ret);

        if (debug_identidy)
            printf("Suceccefully loaded %s!\n", debug_identidy);
        return ret;
    }

    shared_texture loadTexture(void *data, std::size_t size, const char *debug_identidy) {
        if (texture_cache.find({data, size}) != texture_cache.end()) {
            if (auto ret = texture_cache[{data, size}].lock())
                return ret;
        }


        Math::pointi dimentions;

        shared_surface surf = loadSurface(data, size, debug_identidy);
        auto texture = SDL_CreateTextureFromSurface(Graphics::renderer, surf.get());
        auto ret = std::shared_ptr<SDL_Texture>(texture, sdl_deleter());
        texture_cache.insert_or_assign({data, size}, ret);
        return ret;
    }


}