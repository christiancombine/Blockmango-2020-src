#pragma once

#include "Core.h"

namespace LORD
{
	class Object;

	class Component : public LORD::ObjectAlloc
	{
	public:
		virtual ~Component() = default;

		virtual bool Init(Object* pObject) = 0;
		virtual bool Update() = 0;
		virtual bool Shut() = 0;
	};
}