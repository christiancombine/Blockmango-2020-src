#pragma once

#include "Core.h"
#include <vector>

class VarList;

namespace LORD
{
	class Component;

	class Object : public ObjectAlloc
	{
	public:
		virtual ~Object() = default;

		virtual bool Init(const VarList& args);
		virtual bool Update();
		virtual bool Shut();

		bool addComponent(Component* pComponent);
		bool removeComponent(Component* pComponent);

	protected:
		std::vector<Component*> m_vecComponent;
	};
}