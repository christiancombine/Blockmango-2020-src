#pragma once
#ifndef __COMMON_DATA_DISPATCH_H__
#define __COMMON_DATA_DISPATCH_H__

#include "BM_TypeDef.h"

class CommonDataDispatch : public Singleton<CommonDataDispatch>, public ObjectAlloc
{
private:
	typedef std::function<void(ui64, String &, String&, String &)> HandleCommonDataFunc;
	std::map<String, HandleCommonDataFunc>  m_handle_funcs;

public:
	CommonDataDispatch() {};
	~CommonDataDispatch()
	{
		unregisterAll();
	}

	void unregisterAll()
	{
		m_handle_funcs.clear();
	}

	void registerHandle(String name, HandleCommonDataFunc func) 
	{
		if (m_handle_funcs.find(name) == m_handle_funcs.end())
		{
			m_handle_funcs.insert({ name, func });
		}
		else
		{
			assert(0);
		}
	}

	void handleCommonData(ui64 userId, String name, String key, String data)
	{
		auto iter = m_handle_funcs.find(name);
		if (iter != m_handle_funcs.end())
		{
			iter->second(userId, name, key, data);
		}
	}
};

#endif