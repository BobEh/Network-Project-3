#ifndef Entity_h
#define Entity_h

#include <assert.h>
#include <vector>
#include <string>

#include "component.h"

class Entity
{
public:
	Entity(void);
	virtual ~Entity(void);

	template<class T> T* AddComponent();
	template<class T> T* GetComponent(void);
	template<class T> bool HasComponent(void);
	void RemoveComponent(Component* component);

	unsigned int id;							// id of the entity
	std::string name;							// name of the entity
	std::vector<Component*> components;			// Container of components attached to the entity
	std::vector<Entity*> children;
};

typedef std::vector<Entity*>::iterator entity_iterator;

/**
 * AddComponent
 * Adds a component of the provided type to the entity
 * @return - The component that was created
 */
template <class T> T* Entity::AddComponent()
{
	T* newComponent = new T();
	assert(newComponent);

	if (dynamic_cast<Component*>(newComponent) == 0) return 0;		// Check if the provided class type is a Component.
	this->components.push_back(newComponent);

	return newComponent;
}

/**
 * HasComponent
 * Checks the container of components if there is a component of the
 * provided type in it.
 * @return - TRUE if found, FALSE otherwise
 */
template<class T> bool Entity::HasComponent()
{
	for (unsigned int i = 0; i < components.size(); i++)
	{
		if (dynamic_cast<T*>(components[i]) != 0) return true;
	}

	return false;
}

/**
 * GetComponent
 * Checks if the entity contains a component of the provided type
 * @return - The component if found, 0 otherwise
 */
template<class T> T* Entity::GetComponent()
{
	// Should we check if T is a Component type for debugging purposes?

	for (unsigned int i = 0; i < components.size(); i++)
	{
		if (dynamic_cast<T*>(components[i]) != 0)
		{
			return dynamic_cast<T*>(components[i]);
		}
	}

	return 0;		// Entity does not contain the component type
}

#endif