#include "FunctionSetting.h"

namespace BLOCKMAN
{
	FunctionSetting::FunctionSetting()
	{
	}

	FunctionSetting::~FunctionSetting()
	{
		m_configs.clear();
	}
	void FunctionSetting::add(const String & key, bool value)
	{
		m_configs.insert({ key, value });
	}

	bool FunctionSetting::enabledFriend()
	{
		auto config = m_configs.find("friend");
		if (config != m_configs.end())
		{
			return !config->second;
		}
		return true;
	}

	bool FunctionSetting::enabledRecharge()
	{
		auto config = m_configs.find("recharge");
		if (config != m_configs.end())
		{
			return !config->second;
		}
		return true;
	}
}