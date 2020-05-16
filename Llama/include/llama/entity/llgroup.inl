
template<typename EntityType>
void llama::Group::addEntity(EntityType&& entity)
{
    static_assert(std::is_base_of<llama::Entity, EntityType>::value, "EntityType must derive from llama::Entity");

    m_members.insert(std::make_pair(static_cast<Entity>(entity).getUID(), 
                                    std::make_unique<EntityType>(entity)));
}

void llama::Group::removeEntity(UID entity)
{
    m_members.erase(entity);
}

llama::Entity* llama::Group::getEntity(UID entity)
{
    return m_members[entity].get();
}