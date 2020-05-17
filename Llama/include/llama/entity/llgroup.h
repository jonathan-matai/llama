/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llgroup.h
 Contains the Entity Group Class
*/

#pragma once

#include "llentity.h"

namespace llama
{
    class Group : public Entity
    {
        friend class EntityManager_T;
        friend class Renderer_IVulkan;

    public:

        explicit Group(UID uniqueID) :
            Entity(uniqueID)
        {
            m_flags.set(EntityFlags::GROUP);
        }


        // Adds an Entity to the Group
        // @EntityType:     The Type of the Entity, must be a subclass of llama::Entity
        // @entity:         The entity, that should be added
        template<typename EntityType>
        void addEntity(EntityType&& entity);

        // Removes an Entity from the Group
        // @entity:         The UID of the Entity to remove
        inline void removeEntity(UID entity);

        // Get a pointer to an Entity from a UID
        // @entity:         The UID of the entity to retrieve
        inline Entity* getEntity(UID entity);

        inline void onTick(float delta) override;

        auto begin() { return m_members.begin(); }
        auto end() { return m_members.end(); }

    private:

        std::unordered_map<uint64_t, std::unique_ptr<Entity>> m_members;
    };
}

#include "llgroup.inl"