#include "entity.h"
#include "component.h"

/**
 * Constructor
 */
Entity::Entity(void)
{
}

/**
 * Desstructor
 * Handles cleanup of the entity by removing all components
 */
Entity::~Entity(void)
{
	for (unsigned int cmpIndex = 0; cmpIndex < this->components.size(); cmpIndex++)
	{
		RemoveComponent(this->components[cmpIndex]);
	}
}

/**
 * RemoveComponent
 * Removes a component from the entity, if the component is not
 * attached to the entity, nothing happens
 * @param c - pointer to the component to remove
 */
void Entity::RemoveComponent(Component* c)
{
	for (unsigned int i = 0; i < components.size(); i++)
	{
		if (this->components[i] == c)
		{
			delete this->components[i];

			if (this->components.size() > 0)
			{
				this->components[i] = this->components[components.size()];
			}
			this->components.pop_back();
			return;
		}
	}
}