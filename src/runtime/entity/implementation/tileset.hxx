#ifndef _ARCADE_PACTILESET
#define _ARCADE_PACTILESET


#include <runtime/entity/entity.hxx>
#include <rom/rom.hxx>
#include <vector>
#include <graphics/graphics.hxx>
#include <cmath>
#include <extern/stb_image.h>
#include <system/math.hxx>

namespace Runtime {
   namespace Pac {
        constexpr Math::pointi tile_size = Math::pointi(8, 8);
        using movement_tile = Math::pointi;
        enum struct PACDirection: unsigned char {
            RIGHT = 0,
            UP = 1,
            DOWN = 2,
            LEFT = 3,
            LAST,
        };

        constexpr Math::pointi vfromd(PACDirection direction) {
            switch (direction) {
                case PACDirection::RIGHT:   return Math::pointi(1, 0);
                case PACDirection::UP:      return Math::pointi(0, -1);
                case PACDirection::DOWN:    return Math::pointi(0, 1);
                case PACDirection::LEFT:    return Math::pointi(-1, 0);
            }

            return Math::pointi(0, 0);
        }

        constexpr PACDirection dfromv(Math::pointi vector) {\
            if (std::abs(vector.x) > std::abs(vector.y))
                return vector.x>0?  PACDirection::RIGHT : PACDirection::LEFT;
            else return vector.y<0? PACDirection::UP    : PACDirection::DOWN;
        }

        enum struct PACPellet: unsigned char {
            none,
            regular,
            super,
            fire_super
        };


        // TODO: Add dynamic loading for multiple tilemaps. Ms. Pacman... etc...
        struct Tilemap: public Runtime::Entity::Entity {
            Tilemap(Graphics::shared_texture tilemap_image, Graphics::shared_surface tilemap_collision) {
                tilemap_texture =           tilemap_image;
                collectables_texture =      ARCADE_LOADTEXTROM(IMGmazeCollectables);
                int x, y, depth;
                ghost_hut_entrance = Math::pointi{112, 92};
                ghost_hut_home = Math::pointi{112, 120};
                if (SDL_MUSTLOCK(tilemap_collision.get()))
                    SDL_assert_always(SDL_LockSurface(tilemap_collision.get()) == 0);
                SDL_assert_always(tilemap_collision->format->BytesPerPixel == 4);
                for (size_t i = 0; i < (tilemap_collision->w*tilemap_collision->h); i++) {
                    Uint32 pixel = *(Uint32*)((Uint8*)tilemap_collision->pixels + i*tilemap_collision->format->BytesPerPixel);
                    SDL_Color color = { 0 };
                    SDL_GetRGB(pixel, tilemap_collision->format, &color.r, &color.g, & color.b);

                    uint8_t value = color.r;


                    slow_moving_tile.push_back( value == 60);
                    if (value == 255) collision.push_back(true), pellets.push_back(PACPellet::none); 

                    else {
                        collision.push_back(false);
                        switch (value) {
                            case 143:
                                {
                                    auto pellet = std::rand() % 2 == 0? PACPellet::fire_super : PACPellet::super;
                                    pellets.push_back(pellet);
                                }
                                
                                break;

                            default:
    
                                pellets.push_back(PACPellet::regular); break;
                            
                            case 88: 
                                pellets.push_back(PACPellet::none);
                        }
                    }
                    
                }

                SDL_assert(!SDL_QueryTexture(tilemap_texture.get(), nullptr, nullptr, &tilemap_size.x, &tilemap_size.y));
                tilemap_size = tilemap_size / tile_size;

                if (SDL_MUSTLOCK(tilemap_collision.get()))
                    SDL_UnlockSurface(tilemap_collision.get());
            }

            void draw() {
                Entity::draw();
                auto size = tilemap_size*tile_size;
                SDL_Rect dst = {position.x, position.y, size.w, size.h};
                SDL_assert(!SDL_RenderCopy(Graphics::renderer, tilemap_texture.get(), {}, &dst));

                for (int i = 0; i < pellets.size(); i++) {
                    if (pellets[i] != PACPellet::none) {
                        movement_tile tile = {
                            i % tilemap_size.w, 
                            i / tilemap_size.w
                        };

                        auto pos = Math::pointi(tile*tile_size) + position;
                        SDL_Rect dst = {
                            pos.x, pos.y, 8, 8
                        };

                        SDL_Rect src; 
                        switch (pellets[i]) {
                            case PACPellet::regular:
                                src = {0, 0, 8, 8}; break;

                            case PACPellet::super:
                                src = {8, 0, 8, 8}; break;

                            case PACPellet::fire_super:
                                src = {16, 0, 8, 8}; break;
                            default: break;
                        }

                        SDL_assert(!SDL_RenderCopy(Graphics::renderer, collectables_texture.get(), &src, &dst));
                    }
                }
                
            }

            const inline movement_tile getClosestTile(Math::pointi point) const {
                point = (point) - position;
                point = point / tile_size;
                return static_cast<movement_tile>(point);
            }

            const inline Math::pointi getTilePosition(movement_tile tile) const {
                tile = tile * tile_size;
                tile = tile + position + tile_size/2;
                return static_cast<Math::pointi>(tile);
            }

            const inline int getTileIndex(movement_tile tile) const {
                return std::abs(
                    std::clamp(tile.x, 0, tilemap_size.x - 1)
                ) + std::abs(
                    std::clamp(tile.y, 0, tilemap_size.y - 1))*tilemap_size.w;
            }

            const inline bool isBlocked(movement_tile tile) const {
                int tile_index = getTileIndex(tile);
                if (tile_index < 0 || tile_index > collision.size()) return true;
                return collision[tile_index];
            }

            const inline  std::string_view getIdentity() const { return "Tilemap"; }

            Math::pointi position = {};
            Math::pointi tilemap_size = {};

            Math::pointi ghost_hut_entrance = {};
            Math::pointi ghost_hut_home = {};
            bool ghost_hut_busy = false;


            Graphics::shared_texture tilemap_texture = nullptr;
            Graphics::shared_texture collectables_texture = nullptr;
            
            std::vector<bool> slow_moving_tile = {};
            std::vector<bool> collision = {};
            std::vector<PACPellet> pellets = {};     

        };
    }

}


#endif