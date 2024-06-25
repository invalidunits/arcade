#ifndef _ARCADE_SFX
#define _ARCADE_SFX


#include <string>
#include <map>
#include <vector>
#include <rom/rom.hxx>
#include <system/err.hxx>
#include <SDL2/SDL_mixer.h>

namespace Runtime {
    
    namespace Sound {
        struct audiodata {
            std::vector<size_t> positions = {};
            Uint8 *buffer = nullptr;
            Uint32 buffer_len = 0;
        };

        template <const unsigned char *sfx> struct SoundEffect {
            SoundEffect() = delete;
            static bool is_initialized() { return initialized; }
            static void InitializeSFX(size_t sfx_size) {
                SDL_RWops *rwops = SDL_RWFromConstMem(sfx, sfx_size);
                if (audio_chunk = Mix_LoadWAV_RW(rwops, 1), audio_chunk == nullptr) {
                    SDL_FreeRW(rwops);
                    throw sdl_exception();
                }
            }
            

            static void StartSoundMS(int loop = 0, int time = 0) {
                if (time > 5)
                    Mix_PlayChannelTimed(-1, audio_chunk, loop, time);
                else
                    Mix_PlayChannel(-1, audio_chunk, loop);
            }

            template <typename T>
            static void StartSound(int loop, std::chrono::duration<T> time) {
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
                StartSoundMS(loop, ms.count());
            }

            static void StartSound(int loop = 0) {
                StartSoundMS(loop);
            }

            

            private:
                static inline bool initialized = false;
                static inline Mix_Chunk *audio_chunk = nullptr;
        };
    }
} // namespace Runtime


#endif




