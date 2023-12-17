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
        // TODO: Add dynamic loading for multiple tilemaps. Ms. Pacman... etc...
        struct Tilemap: public Runtime::Entity::Entity {
            Tilemap() {
                tilemap_texture =   Graphics::loadTexture((void *)ROM::gIMGmazeData, ROM::gIMGmazeSize);
                int x, y, depth;
                auto collide = stbi_load_from_memory((stbi_uc *)ROM::gIMGmazeCollisionData, ROM::gIMGmazeCollisionSize, &x, &y, &depth, 1);
                for (size_t i = 0; i < (x*y); i++) {
                    collision.push_back(collide[i] != 0);
                }
                stbi_image_free(collide);

                SDL_assert(!SDL_QueryTexture(tilemap_texture.get(), nullptr, nullptr, &tilemap_size.x, &tilemap_size.y));
                tilemap_size = tilemap_size / tile_size;
            }

            void draw() {
                Entity::draw();
                auto size = tilemap_size*tile_size;
                SDL_Rect dst = {position.x, position.y, size.w, size.h};
                SDL_assert(!SDL_RenderCopy(Graphics::renderer, tilemap_texture.get(), {}, &dst));
            }

            const movement_tile getClosestTile(Math::pointi point) const {
                point = point - position;
                point = point / tile_size;
                return static_cast<movement_tile>(point);
            }

            const Math::pointi getTilePosition(movement_tile tile) const {
                tile = tile + position;
                tile = tile * tile_size;
                return static_cast<Math::pointi>(tile);
            }

            const bool isBlocked(movement_tile tile) const {
                // You're not allowed to leave the screen.
                if (tile.x < 0 || tile.x > tilemap_size.w) return true;
                if (tile.y < 0 || tile.y > tilemap_size.h) return true;
                
                int tileindex = std::abs(tile.x) + std::abs(tile.y)*tilemap_size.w;
                if (tileindex < 0 || tileindex > collision.size()) return true;


                return collision[tileindex];
            }

            const std::string_view getIdentity() const { return "Tilemap"; }

            Math::pointi position = {};
            Math::pointi tilemap_size = {};
            Graphics::shared_texture tilemap_texture = nullptr;
            std::vector<bool> collision = {};

        };
    }

}


#endif