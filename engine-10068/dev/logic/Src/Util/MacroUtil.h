#pragma once

#ifndef NULL_RETURN_VOID
#define NULL_RETURN_VOID(p)\
if ((p) == nullptr)\
{\
	return;\
}
#endif

#ifndef NULL_RETURN
#define NULL_RETURN(p, v)\
if ((p) == nullptr)\
{\
	return (v);\
}
#endif

#ifndef NULL_CONTINUE
#define NULL_CONTINUE(p)\
if ((p) == nullptr)\
{\
	continue;\
}
#endif

static const int maxLoopCount = 100000; //最大循环次数

#define LoopCheckBegin(c) \
int loopCount##c = 0;

#define LoopCheckDo(c) \
if (loopCount##c >= maxLoopCount)\
{\
	break;\
}\
++loopCount##c;

#ifndef CHECK_TIME_INTERVAL_VOID
#define CHECK_TIME_INTERVAL_VOID(ms)\
static auto time1 = Root::Instance()->getCurrentTime();\
auto time2 = Root::Instance()->getCurrentTime();\
if (time2 - time1 < ms)\
{\
	return;\
}\
time1 = time2;
#endif

#ifndef CHECK_TIME_INTERVAL
#define CHECK_TIME_INTERVAL(ms, ret)\
static auto time1 = Root::Instance()->getCurrentTime();\
auto time2 = Root::Instance()->getCurrentTime();\
if (time2 - time1 < ms)\
{\
	return (ret);\
}\
time1 = time2;
#endif

#ifndef GETINSTANCE
#define GETINSTANCE(CLASS)\
public:\
	static CLASS& getInstance()\
	{\
		static CLASS instance; \
		return instance; \
	}
#endif

#ifndef DECLARE_PROPERTY_MAP
#define DECLARE_PROPERTY_MAP	\
public:\
	bool HasProperty(const std::string& key)\
	{\
		return m_mapProperty.find(key) != m_mapProperty.end();\
	}\
\
	template <class T>\
	T GetProperty(const std::string& key)\
	{\
		auto it = m_mapProperty.find(key);\
		if (it == m_mapProperty.end())\
		{\
			assert(0);\
			throw;\
		}\
		return any_cast<T>(it->second);\
	}\
\
	template <class T>\
	void SetProperty(const std::string& key, const T& value)\
	{\
		auto it = m_mapProperty.find(key);\
		if (it != m_mapProperty.end())\
		{\
			if (it->second.type() != typeid(T))\
				return;\
			m_mapProperty.erase(it);\
		}\
		m_mapProperty.emplace(std::make_pair(key, value));\
	}\
\
protected:\
	std::unordered_map<std::string, Any> m_mapProperty;
#endif