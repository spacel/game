#include "gameobject.h"

namespace spacel {

namespace engine {

GameObject::GameObject(GameObjectType type) : m_go_type(type)
{
	m_object_typemask |= OBJECT_TYPEMASK_GAMEOBJECT;
	m_type = OBJECT_TYPE_GAMEOBJECT;
}

}
}
