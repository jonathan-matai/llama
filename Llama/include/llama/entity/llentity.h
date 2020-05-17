/*
  _    _
 | |  | |__ _ _ __  __ _
 | |__| / _` | '  \/ _` |
 |____|_\__,_|_|_|_\__,_|

 Llama Game Library

 > llentity.h
 Contains the Base Entity Class
*/

#pragma once

#include "../graphics/llbuffer.h"

namespace llama
{
    LLAMA_CLASS_DECLARATION(Shader);
    LLAMA_CLASS_DECLARATION(ConstantSet);

    using UID = uint64_t;

    enum class EntityFlags
    {
        RENDERABLE = bit(1),
        SOUNDEMITTING = bit(2),
        GROUP = bit(3)
    };

    class Entity
    {
        friend class EntityManager_I;
        friend class Renderer_IVulkan;

    public:

        explicit Entity(UID uniqueID) :
            m_uniqueID(uniqueID)
        { }

        virtual ~Entity() = default;

        virtual void onTick(float delta) { }

        UID getUID() const { return m_uniqueID; }

    protected:

        Flags<EntityFlags> m_flags;

        const UID m_uniqueID;
    };

    class RenderableEntity : public Entity
    {
        friend class Renderer_IVulkan;

    public:

        RenderableEntity(UID uniqueID,
                         Shader shader,
                         VertexBuffer vertexBuffer,
                         ConstantSet constantSet,
                         ConstantArrayBuffer constantBuffer,
                         IndexBuffer indexBuffer = nullptr) :
            Entity(uniqueID),
            m_shader(shader),
            m_vertexBuffer(vertexBuffer),
            m_constantSet(constantSet),
            m_constantBuffer(constantBuffer),
            m_indexBuffer(indexBuffer)
        {
            m_flags.set(EntityFlags::RENDERABLE);
            m_arrayIndex = m_constantBuffer->addElement();
        }

        virtual ~RenderableEntity()
        {
            if(m_arrayIndex != UINT32_MAX)
                m_constantBuffer->removeElement(m_arrayIndex);
        }

        RenderableEntity(const RenderableEntity& e) = delete;

        RenderableEntity(RenderableEntity&& e) :
            Entity(e.m_uniqueID),
            m_shader(e.m_shader),
            m_vertexBuffer(e.m_vertexBuffer),
            m_constantSet(e.m_constantSet),
            m_constantBuffer(e.m_constantBuffer),
            m_indexBuffer(e.m_indexBuffer),
            m_arrayIndex(e.m_arrayIndex)
        {
            m_flags = e.m_flags;
            e.m_arrayIndex = UINT32_MAX;
        }

    protected:

        Shader m_shader;
        VertexBuffer m_vertexBuffer;
        ConstantSet m_constantSet;
        ConstantArrayBuffer m_constantBuffer;
        IndexBuffer m_indexBuffer;
        uint32_t m_arrayIndex;
    };
}