#pragma once
#include <list>
#include "Component.h"
#include "Transform.h"
#include "SystemManager.h"
class Entity
{
private: 
	int _id;
	std::list<Component*> _components;
public:
	Transform transform;
	Entity();
	
	const int GetId();

	template<typename T>
	void AddComponent() {
		T* t = new T();
		t->SetOwner(this);
		_components.push_back(t);
	}

	template<typename T>
	T* GetComponent()
	{
		for (Component* c : _components) {

			T* t = dynamic_cast<T*>(c);
			if (t != nullptr)
			{
				return t;
			}
		}
		return nullptr;
	}

	virtual ~Entity();
};

