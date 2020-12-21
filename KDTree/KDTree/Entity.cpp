#include "Entity.h"

	Entity::Entity() :
		_id(0), transform(Transform())
	{
	}

	const int Entity::GetId()
	{
		return _id;
	}

	Entity::~Entity() {
		for (Component* c : _components) {
			delete c;
		}
		_components.clear();
	}

