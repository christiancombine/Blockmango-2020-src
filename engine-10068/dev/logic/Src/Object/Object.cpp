#include "Object.h"
#include "Util/MacroUtil.h"
#include "Any/VarList.h"
#include "Component/Component.h"

namespace LORD
{
	bool Object::Init(const VarList & args)
	{
		return false;
	}

	bool Object::Update()
	{
		return false;
	}

	bool Object::Shut()
	{
		return false;
	}

	bool Object::addComponent(Component* pComponent)
	{
		NULL_RETURN(pComponent, false);
		pComponent->Init(this);
		m_vecComponent.emplace_back(pComponent);
		return true;
	}

	bool Object::removeComponent(Component* pComponent)
	{
		NULL_RETURN(pComponent, false);
		for (auto it = m_vecComponent.begin(); it != m_vecComponent.end(); ++it)
		{
			if (*it == pComponent)
			{
				pComponent->Shut();
				m_vecComponent.erase(it);
				return true;
			}
		}
		return false;
	}
}
