#pragma once
class Entity;

class Component
{
public:
	virtual void Update() = 0;
	virtual ~Component();
	void SetOwner(Entity* owner);
	Entity* GetOwner();

protected:
	Component();
	Entity* _owner = nullptr;
};

