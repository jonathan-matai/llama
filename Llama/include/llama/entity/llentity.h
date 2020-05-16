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

    class Entity
    {
    public:

        enum class TypeBits
        {
            RENDERABLE = bit(1),
            SOUNDEMITTING = bit(2)
        };

        Entity(UID uniqueID) :
            m_uniqueID(uniqueID)
        { }

        virtual ~Entity() = default;

        virtual void onTick(float delta) { }

        UID getUID() const { return m_uniqueID; }

    protected:

        Flags<TypeBits> m_flags;

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
            m_flags.set(TypeBits::RENDERABLE);
            m_arrayIndex = m_constantBuffer->addElement();
        }

        virtual ~RenderableEntity()
        {
            m_constantBuffer->removeElement(m_arrayIndex);
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