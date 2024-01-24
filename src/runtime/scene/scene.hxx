#if !defined(_ARCADE_SCENE)
#define _ARCADE_SCENE
#include <cstdint>
#include <memory>
#include <map>
#include <vector>
#include <type_traits>
#include <system/math.hxx>

#include <system/clock.hxx>
#include <any>

namespace Runtime {
    struct IUpdate {
        virtual void setup(void) {}
        virtual void cleanup(void) {}

        virtual void update_fixed(void) {}
        virtual void update(void) {}
        virtual void draw(void) {}
    };

    struct Scene : virtual public IUpdate {
        virtual void pause(void) {}
        virtual void resume(void) {}
    };

    namespace SceneManager {
        inline std::vector<Math::UUID> scene_stack = {};
        inline std::map<Math::UUID, std::unique_ptr<Scene>> scene_map = {};   
       


        template <typename T> Math::UUID registerScene() {
            static_assert(std::is_base_of_v<Scene, T>, "Type must be scene for it to be registered.");

            if (scene_map.find(Math::getUUID<T>()) == scene_map.end()) {
                scene_map.insert_or_assign(Math::getUUID<T>(), std::make_unique<T>());
            }

            return Math::getUUID<T>();
        }

        static Scene *getCurrentScene(void) {
            if (scene_stack.empty()) {
                registerScene<Scene>();
                return scene_map.at(Math::getUUID<Scene>()).get(); // Return an empty scene.
            }

            return scene_map.at(scene_stack.back()).get(); 
        }

        static Math::UUID popScene(void) {
            getCurrentScene()->cleanup();
            auto id = scene_stack.back();
            scene_stack.pop_back();
            return id;
        }

        template<typename T> void pushScene(void) {
            registerScene<T>();
            getCurrentScene()->pause();
            scene_stack.push_back(Math::getUUID<T>());
            getCurrentScene()->setup();
        }

        template<typename T> void gotoScene(void) {
            registerScene<T>();
            popScene();
            pushScene<T>();
        }




    }
} // namespace Runtime






#endif // _ARCADE_SCENE
