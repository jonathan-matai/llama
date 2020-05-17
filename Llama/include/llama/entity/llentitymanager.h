/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llentitymanager.h
 Contains the Base Entity Class
*/

#pragma once

#include "llgroup.h"
#include "../event/llevent.h"

namespace llama
{
    class EntityManager_T
    {
        friend class Renderer_IVulkan;

    public:

        EntityManager_T(EventNode node) :
            m_entities(0),
            m_onTick(node, this, &EntityManager_T::onTick)
        { }

        // Adds an Entity to the Group
        // @EntityType:     The Type of the Entity, must be a subclass of llama::Entity
        // @entity:         The entity, that should be added
        template<typename EntityType>
        void addEntity(EntityType&& entity)
        {
            static_assert(std::is_base_of<llama::Entity, EntityType>::value, "EntityType must derive from llama::Entity");

            m_entities.m_members.insert(std::make_pair(static_cast<Entity>(entity).getUID(),
                                                       std::make_unique<EntityType>(std::move(entity))));
        }

        llama::EventDispatchState onTick(TickEvent* e)
        {
            m_entities.onTick(e->m_deltaTime);
            return EventDispatchState::PROCESSED;
        }

    protected:

        EventDispatchFunction m_onTick;
        Group m_entities;
    };

    LLAMA_CLASS_DECLARATION(EntityManager);

    inline EntityManager createEntityManager(EventNode node)
    {
        return std::make_shared<EntityManager_T>(node);
    }
}