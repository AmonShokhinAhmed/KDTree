#include "Component.h"

Component::Component()
{

}
void Component::SetOwner(Entity* owner)
{
	_owner = owner;
}

Entity* Component::GetOwner()
{
	return _owner;
}