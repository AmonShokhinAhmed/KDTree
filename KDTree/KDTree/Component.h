#pragma once
class Entity;

class Component
{
public:
	virtual void Update() = 0;
	void SetOwner(Entity* owner);
	Entity* GetOwner();

protected:
	Component();
	Entity* _owner = nullptr;
};

