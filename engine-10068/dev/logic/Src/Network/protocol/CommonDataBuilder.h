#pragma once
#ifndef __COMMON_DATA_BUILDER_H__
#define __COMMON_DATA_BUILDER_H__

#include "Core.h"

using namespace LORD;

class CommonDataBuilder
{
private:
	map<String, String>::type params;

public:
	CommonDataBuilder& fromData(String data)
	{
		StringArray array = StringUtil::Split(data, "&");
		for (auto item : array)
		{
			StringArray param = StringUtil::Split(item, "=", 2);
			if (param.size() == 2)
			{
				params.insert({ param[0], param[1] });
			}
		}
		return *this;
	}

	CommonDataBuilder& fromListData(String data)
	{
		StringArray array = StringUtil::Split(data, "&");	
		for (int i = 0; i < array.size(); i++)
		{
			params.insert({ StringUtil::ToString(i), array[i] });
		}
		return *this;
	}

	CommonDataBuilder& addIntParam(String key, int value)
	{
		params.insert({ key, StringUtil::ToString(value) });
		return *this;
	}

	CommonDataBuilder& addFloatParam(String key, float value)
	{
		params.insert({ key, StringUtil::ToString(value) });
		return *this;
	}

	CommonDataBuilder& addDoubleParam(String key, double value)
	{
		params.insert({ key, StringUtil::ToString(value) });
		return *this;
	}

	CommonDataBuilder& addBoolParam(String key, bool value)
	{
		params.insert({ key, StringUtil::ToString(value) });
		return *this;
	}

	CommonDataBuilder& addLongParam(String key, long value)
	{
		params.insert({ key, StringUtil::ToString(value) });
		return *this;
	}

	CommonDataBuilder& addI64Param(String key, i64 value)
	{
		params.insert({ key, StringUtil::ToString(value) });
		return *this;
	}

	CommonDataBuilder& addStringParam(String key, String value)
	{
		params.insert({ key, value });
		return *this;
	}

	bool hasParam(const String& key)
	{
		auto param = params.find(key);
		return param != params.end();
	}

	CommonDataBuilder& addVector2Param(String key, Vector2 value)
	{
		params.insert({ key, StringUtil::Format("%f:%f", value.x, value.y) });
		return *this;
	}

	CommonDataBuilder& addVector2iParam(String key, Vector2i value)
	{
		params.insert({ key, StringUtil::Format("%d:%d", value.x, value.y) });
		return *this;
	}

	CommonDataBuilder& addVector3Param(String key, Vector3 value)
	{
		params.insert({ key, StringUtil::Format("%f:%f:%f", value.x, value.y, value.x) });
		return *this;
	}

	CommonDataBuilder& addVector3iParam(String key, Vector3i value)
	{
		params.insert({ key, StringUtil::Format("%d:%d:%d", value.x, value.y, value.x) });
		return *this;
	}

	int getIntParam(String key)
	{
		auto param = params.find(key);
		if (param != params.end())
		{
			return StringUtil::ParseI32(param->second);
		}
		return 0;
	}

	float getFloatParam(String key)
	{
		auto param = params.find(key);
		if (param != params.end())
		{
			return StringUtil::ParseFloat(param->second);
		}
		return 0.f;
	}

	double getDoubleParam(String key)
	{
		auto param = params.find(key);
		if (param != params.end())
		{
			return StringUtil::ParseDouble(param->second);
		}
		return 0.0;
	}

	bool getBoolParam(String key)
	{
		auto param = params.find(key);
		if (param != params.end())
		{
			return StringUtil::ParseBool(param->second);
		}
		return false;
	}

	long getLongParam(String key)
	{
		auto param = params.find(key);
		if (param != params.end())
		{
			return StringUtil::ParseLong(param->second);
		}
		return 0;
	}

	i64 getI64Param(String key)
	{
		auto param = params.find(key);
		if (param != params.end())
		{
			return StringUtil::ParseI64(param->second);
		}
		return 0;
	}

	ui64 getUI64Param(String key)
	{
		auto param = params.find(key);
		if (param != params.end())
		{
			return StringUtil::ParseUI64(param->second);
		}
		return 0;
	}

	String getStringParam(String key)
	{
		auto param = params.find(key);
		if (param != params.end())
		{
			return param->second;
		}
		return "";
	}

	ui32 getUI32Param(String key)
	{
		auto param = params.find(key);
		if (param != params.end())
		{
			return StringUtil::ParseUI32(param->second);
		}
		return 0;
	}

	Vector2 getVector2(String key)
	{
		auto param = params.find(key);
		if (param != params.end())
		{
			StringArray array = StringUtil::Split(param->second, ":");
			if (array.size() == 2)
			{
				return Vector2(StringUtil::ParseFloat(array[0]), StringUtil::ParseFloat(array[1]));
			}
		}
		return Vector2::ZERO;
	}

	Vector2i getVector2i(String key)
	{
		auto param = params.find(key);
		if (param != params.end())
		{
			StringArray array = StringUtil::Split(param->second, ":");
			if (array.size() == 2)
			{
				return Vector2i(StringUtil::ParseI32(array[0]), StringUtil::ParseI32(array[1]));
			}
		}
		return Vector2i::ZERO;
	}

	Vector3 getVector3(String key)
	{
		auto param = params.find(key);
		if (param != params.end())
		{
			StringArray array = StringUtil::Split(param->second, ":");
			if (array.size() == 3)
			{
				return Vector3(StringUtil::ParseFloat(array[0]), StringUtil::ParseFloat(array[1]), StringUtil::ParseFloat(array[2]));
			}
		}
		return Vector3::ZERO;
	}

	Vector3i getVector3i(String key)
	{
		auto param = params.find(key);
		if (param != params.end())
		{
			StringArray array = StringUtil::Split(param->second, ":");
			if (array.size() == 3)
			{
				return Vector3i(StringUtil::ParseI32(array[0]), StringUtil::ParseI32(array[1]), StringUtil::ParseI32(array[2]));
			}
		}
		return Vector3i::ZERO;
	}

	String getData()
	{
		String data = "";
		for (auto param : params)
		{
			String item = param.first + "=" + param.second;
			appendParam(data, item);
		}
		return data;
	}

	String getListData()
	{
		String data = "";
		for (auto param : params)
		{
			appendParam(data, param.second);
		}
		return data;
	}

private:

	void appendParam(String& data, String param)
	{
		if (data.empty())
			data += param;
		else
			data += ("&" + param);
	}
};

#endif // !__COMMON_DATA_BUILDER_H__

