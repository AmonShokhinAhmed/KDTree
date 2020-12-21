#pragma once
#include <vector>
#include "Component.h"

template<class T>
class System
{
protected:
	std::vector<T*> _components;
public:
	
	virtual void Update() = 0;

	void AddComponent(T* component) {
		_components.push_back(component);
	}

	void RemoveComponent(T* component) {
		int index = -1;
		for (int i = 0; i < _components.size(); i++) {
			if (_components.at(i) == component) {
				index = i;
			}
		}
		if (index != -1) {
			_components.erase(_components.begin() + index);
		}
	}

	std::vector<T*> GetComponentList() {
		return _components;
	}
};

