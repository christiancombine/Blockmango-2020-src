/********************************************************************
filename: 	Log.h
file path:	dev\engine\Src\Core\Object\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/
#pragma once

#ifndef __LORD_LOG_H__
#define __LORD_LOG_H__

#include "Memory/LordMemory.h"
#include "g3log/g3log.hpp"
#include "LogConfig.h"

struct lua_State;

namespace LORD
{
	class LORD_CORE_API Log
	{
	public:
		// void(const char* message, const char* fileContent, int fileSize, const char* fileName)
		using ErrorFunction = std::function<void(const char*, const char*, int, const char*)>;

	public:
		Log() = delete;

		static bool isInitialized() { return m_initialized; }
		static void setReportErrorFunc(ErrorFunction func) { reportErrorFunc = func; }
		static void init(bool forecLogAppend, bool isClientMode, const String& logCfgFile = "LordLog.cfg");
		static void uninit();
		static void initSynchronousLog();
		static void logSynchronously(const LEVELS& level, const char* format, ...);
		template<typename StringType>
		static String format(LEVELS level, const StringType& message)
		{
			return format(level, String(message.c_str()));
		}
		static String format(LEVELS level, const String& message);
		static LogConfig getConfig() { return config; }
		static String getCallStack() { return g3::internal::stackdump().c_str(); }
		static void printCallStack() { LOG(G3LOG_INFO) << "******* STACKDUMP *******\n" << getCallStack(); }
		static void reportError(const char* format, ...);
		static void reportErrorWithFile(const char* filePath, const char* format, ...);
		static void reportErrorWithFile(const char* content, int length, const char* fileName, const char* format, ...);
		static void reportErrorWithResource(const char* resourceName, const char* format, ...);

		static void appendScriptError(const String& errorText) { m_scriptError += errorText; }
		static String getScriptError() { return m_scriptError; }
		static void clearScriptError() { m_scriptError = ""; }
		static void setLuaState(lua_State* L);
		static void printLuaStack();

	private:
		static void reportErrorInternal(const char* fileContent, int fileSize, const char* fileName, const char* format, va_list argp);

	private:
		static LogConfig config;
		static bool m_initialized;
		static bool m_isClientMode;
		static String m_scriptError;
		static bool hasReportError;
		static ErrorFunction reportErrorFunc;
	};
}

#define LordLogDebug(formats, ...)      LOGF(G3LOG_DEBUG, formats, ##__VA_ARGS__);
#define LordLogInfo(formats, ...)       LOGF(G3LOG_INFO, formats, ##__VA_ARGS__);
#define LordLogWarning(formats, ...)    LOGF(G3LOG_WARNING, formats, ##__VA_ARGS__);
#define LordLogError(formats, ...)      LOGF(G3LOG_ERROR, formats, ##__VA_ARGS__);
#define LordLogFatal(formats, ...)      LOGF(G3LOG_FATAL, formats, ##__VA_ARGS__);
#define LordLogReportError(formats, ...)  LORD::Log::reportError(formats, ##__VA_ARGS__); LOGF(G3LOG_ERROR, formats, ##__VA_ARGS__);
#define LordLogReportErrorFile(filePath, formats, ...)  LORD::Log::reportErrorWithFile(filePath, formats, ##__VA_ARGS__); LOGF(G3LOG_ERROR, formats, ##__VA_ARGS__);
#define LordLogReportErrorResource(resourceName, formats, ...)  LORD::Log::reportErrorWithResource(resourceName, formats, ##__VA_ARGS__); LOGF(G3LOG_ERROR, formats, ##__VA_ARGS__);

#define LordLogLineSync(msg, ...) ::LORD::Log::logSynchronously(G3LOG_DEBUG, "%s:%d[%s] " msg, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);

#define LordLogInfoOnce(formats, ...)   \
		{                               \
			static bool needLog = true; \
			if(needLog)                 \
            {                           \
			    LOGF(G3LOG_INFO, formats, ##__VA_ARGS__);\
				needLog = false;        \
			}                           \
        }                          
#endif
