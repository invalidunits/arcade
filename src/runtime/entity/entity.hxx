#ifndef _ARCADE_ENTITY
#define _ARCADE_ENTITY

#include <system/math.hxx>
#include <string>
#include <memory>
#include <type_traits>
#include <runtime/scene/scene.hxx>

namespace Runtime {
    namespace Entity {
        struct Entity;
        struct EntityManager;

        struct Component: public IUpdate {
            Component(const Entity *entity): m_entity(entity) {}

                inline const Entity * getEntity() const { return m_entity; }
            protected:
                const Entity *m_entity = nullptr;
        };

        struct Entity : public IUpdate {
            Entity() {}

            template<typename T> T *getComponent() const { 
                static_assert(std::is_base_of_v<Component, T>, "Type must be component");
                const auto component = m_components.find(Math::getUUID<T>());
                if (component == m_components.end()) return nullptr;
                return static_cast<T*>(component->second.get());
            }

            const inline virtual std::string_view getIdentity() const { return "Undefined"; }
            #define __ENTITY_IUPDATE_IMPL(func) void func () { \
                for (const auto &component : m_components) { \
                    component.second->func();\
                }   \
            }

            __ENTITY_IUPDATE_IMPL(setup);
            __ENTITY_IUPDATE_IMPL(update);
            __ENTITY_IUPDATE_IMPL(update_fixed);
            __ENTITY_IUPDATE_IMPL(cleanup);
            __ENTITY_IUPDATE_IMPL(draw);

            #undef __ENTITY_IUPDATE_IMPL

            EntityManager *getManager() const { return m_manager; }

            protected:
                friend struct EntityManager;
                template<typename T, typename... Args> T *registerComponent(Args... args) { 
                    static_assert(std::is_base_of_v<Component, T>, "Type must be component");
                    const auto component = m_components.find(Math::getUUID<T>());
                    if (component != m_components.end()) return static_cast<T*>(component->second.get());
                    m_components.insert_or_assign(Math::getUUID<T>(), std::unique_ptr<Component>((Component *)new T(this, args...)));
                    return getComponent<T>();
                }

                std::map<Math::UUID, std::unique_ptr<Component>> m_components = {};
                EntityManager *m_manager = nullptr;
        };

 


        struct EntityManager: virtual public Scene {
            #define __ENTITY__MANAGER_IUPDATE_IMPL(func) void func () { \
                for (const auto &entity : m_entities) { \
                    entity->m_manager = this; \
                    entity->func();\
                }   \
            }

            void cleanup() {
                EntityManager::m_entities.clear();
            }

            
            __ENTITY__MANAGER_IUPDATE_IMPL(update);
            __ENTITY__MANAGER_IUPDATE_IMPL(update_fixed);
            __ENTITY__MANAGER_IUPDATE_IMPL(draw);


            #undef __ENTITY__MANAGER_IUPDATE_IMPL

            std::vector<Entity *> getEntitysFromID(std::string_view identifier) const {
                std::vector<Entity *> ret = {};
                for (auto it = m_entities.begin(); it != m_entities.end(); it++) 
                    if (it->get()->getIdentity() == identifier) ret.push_back(it->get());
                return ret;
            }
            
            template <typename T, typename... TArgs> T *addEntity(TArgs&&... args) {
                static_assert(std::is_base_of_v<Entity, T>, "Type must be Entity");
                
                
                m_entities.push_back(std::make_unique<T>(args...));
                auto entity = m_entities.back().get();
                entity->m_manager = this;
                entity->setup();

                return static_cast<T*>(entity);
            }
            void removeEntity(Entity *entity) {
                const auto it = std::find_if(m_entities.begin(), m_entities.end(), [&](std::unique_ptr<Entity>& value){
                    return value.get() == entity;
                });
                if (it != m_entities.end()) {
                    entity->cleanup();
                    m_entities.erase(it);
                }
            }

            
            protected:
                std::vector<std::unique_ptr<Entity>> m_entities = {};
        };
    }
}


#endif