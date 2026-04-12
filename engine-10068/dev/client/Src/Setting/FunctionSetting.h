#pragma once
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	
	class FunctionSetting : public Singleton<FunctionSetting>, public ObjectAlloc
	{
	public:
		FunctionSetting();
		~FunctionSetting();

	public:
		void add(const String& key, bool value);
		bool enabledFriend();
		bool enabledRecharge();

	private:
		map<String, bool>::type m_configs;
	};
}