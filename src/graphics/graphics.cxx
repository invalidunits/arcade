
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
    std::vector<std::string> split(std::string_view text, const std::string_view delimiter) {
        std::vector<std::string> ret = {};
        int start = 0, end = 0;
        while ((start = text.find_first_not_of(delimiter.data(), end)) != std::string::npos) {
            end = text.find(delimiter, end);
            ret.push_back(std::string(text.substr(start, end - start)));
        }
        if (ret.empty()) {
            return { std::string(text) };
        }
        return ret;
    }

    std::vector<std::string> splitIntoLines(const std::string_view& str) {
        std::vector<std::string> lines;
        size_t start = 0;
        size_t end = str.find('\n');
        
        while (end != std::string::npos) {
            lines.push_back(std::string(str.substr(start, end - start)));
            start = end + 1;
            end = str.find('\n', start);
        }
        
        // Add the last line (if there is no trailing newline)
        lines.push_back(std::string(str.substr(start)));
        
        return lines;
    }



    void drawText(Math::recti bounds, std::string_view text, SDL_Renderer *renderer,
        Math::color8a color, bool center, TTF_Font *font) {
        int err = 0;
        if (font == nullptr) {
            font = default_font;
        }
        
        std::vector<std::string> string_lines = splitIntoLines(text);
        if (std::min(bounds.w, bounds.h) == 0) {
            err = TTF_SizeText(font, string_lines[0].c_str(), &bounds.w, &bounds.h);
            if (err) throw sdl_exception();
        } 
        
        auto line_skip = TTF_FontLineSkip(font);
        auto text_bounds = bounds;
        for (auto string : string_lines) {
            if (!string.empty()) {
                err = TTF_SizeText(font, string.c_str(), &text_bounds.w, &text_bounds.h);
                if (err) throw sdl_exception();



                SDL_Surface *surface = TTF_RenderText_Solid_Wrapped(font, string.c_str(), 
                    SDL_Color{color.r, color.b, color.g, color.a}, text_bounds.w);

                if (center) {
                    text_bounds.x = bounds.x - surface->w/2;
                    text_bounds.w = surface->w;
                    text_bounds.h = surface->h;
                }

                if (surface == nullptr) throw sdl_exception();

                auto texture = SDL_CreateTextureFromSurface(renderer, surface);
                if (texture == nullptr) throw sdl_exception();
                
                auto rect = (SDL_Rect)text_bounds;
                if (SDL_RenderCopy(renderer, texture, nullptr, &rect)) throw sdl_exception();
                SDL_DestroyTexture(texture);
                SDL_FreeSurface(surface);
            }

            text_bounds.y += line_skip;
        }
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