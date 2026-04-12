#include "Core.h"
#include "Util/StringUtil.h"
#include "Log.h"
#include "LogConfig.h"
#include "Object/Root.h"
#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"
#include "g3log/loglevels.hpp"
#include "ConsoleLogSink.h"
#include "VSConsoleLogSink.h"
#include "FileLogSink.h"
#include "Util\UThread.h"

#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
#include "windows.h"
#endif

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace LORD
{
	LogConfig Log::config;
	bool Log::m_initialized = false;
	bool Log::m_isClientMode = true;
	String Log::m_scriptError = "";
	static std::unique_ptr<g3::LogWorker>* logWorker = nullptr;

	bool Log::hasReportError = false;
	Log::ErrorFunction Log::reportErrorFunc;

	static lua_State* luaState = nullptr;

	static std::string _getLuaStack()
	{
		if (!luaState)
			return "";

		std::string msg;
		// call debug.tracebak() 
		lua_getglobal(luaState, "debug");
		lua_getfield(luaState, -1, "traceback");
		int error = lua_pcall(luaState, 0, 1, 0);
		msg.append(lua_tostring(luaState, -1));
		lua_pop(luaState, 2);

		return msg;
	}

	void Log::init(bool forecLogAppend, bool isClientMode, const String& logCfgFile)
	{
		if (m_initialized)
		{
			return;
		}

		m_isClientMode = isClientMode;
		String configPath;
		if (!PathUtil::IsAbsolutePath(logCfgFile))
		{
			configPath = PathUtil::ConcatPath(Root::Instance()->getRootPath(), logCfgFile);
		}
		else
		{
			configPath = logCfgFile;
		}

		if (logWorker == nullptr)
		{
			logWorker = new std::unique_ptr<g3::LogWorker>;
			*logWorker = std::move(g3::LogWorker::createLogWorker());


			g3::initializeLogging((*logWorker).get());
			config = LogConfig::readConfig(forecLogAppend, m_isClientMode, configPath);

			g3::log_levels::setHighest(config.logLevel);
			for (auto& logDisable : config.logDisable)
			{
				g3::log_levels::disable(logDisable);
			}
			if (config.bConsoleOutput)
			{
				(*logWorker)->addSink(std2::make_unique<ConsoleLogSink>(), &ConsoleLogSink::logMessage);
			}
			if (config.bVSOutput)
			{
				(*logWorker)->addSink(std2::make_unique<VSConsoleLogSink>(), &VSConsoleLogSink::logMessage);
			}
			if (config.bFileOutput)
			{
				(*logWorker)->addSink(std2::make_unique<FileLogSink>(), &FileLogSink::logMessage);
			}
		}
		/*LogCapture::SetAdditionFatalInfo(_getLuaStack);*/
		m_initialized = true;
	}

	void Log::uninit()
	{
		m_initialized = false;
	}

	void Log::initSynchronousLog()
	{
		auto logPath = Root::Instance()->getWriteablePath() + "synchronous.log";
		PathUtil::DelFilesInDir(logPath);
	}

	void Log::logSynchronously(const LEVELS& level, const char* format, ...)
	{
		auto logPath = Root::Instance()->getWriteablePath() + "synchronous.log";
		std::ofstream logFile;
		logFile.open(logPath.c_str(), std::ios_base::out | std::ios_base::app);
		while (!logFile)
		{
			LordLogError("Failed to open synchronous log, thread id:%d, msg:%s", UThread::getThisThreadNativeId(), strerror(errno));
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			logFile.open(logPath.c_str(), std::ios_base::out | std::ios_base::app);
		}
		char buf[512] = {};

		va_list vl;
		va_start(vl, format);
		vsnprintf(buf, sizeof(buf), format, vl);
		va_end(vl);
		logFile << Log::format(level, String(buf));
		logFile.flush();
	}

	String Log::format(LEVELS level, const String& message)
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		switch (level.value)
		{
		case 100:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case 300:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x00F | FOREGROUND_INTENSITY);
			break;
		case 500:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x006 | FOREGROUND_INTENSITY);
			break;
		case 800:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case 1000:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		default:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x00F | FOREGROUND_INTENSITY);
			break;
		}
#endif
		String msgStr;

		if (config.bTimeStamp)
		{
			struct tm* pTime;
			time_t ctTime;
			time(&ctTime);
			pTime = localtime(&ctTime);
			msgStr += StringUtil::Format("%d-%02d-%02d %02d:%02d:%02d ", pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
		}
		msgStr += ("[" + level.text + "]: ").c_str();
		msgStr += message;
		msgStr = "(" + config.logName + ") " + msgStr + "\n";
		return msgStr;
	}

	void Log::setLuaState(lua_State* L)
	{
		luaState = L;
	}

	void Log::printLuaStack()
	{
		std::string stack = _getLuaStack();
		LordLogError("%s", stack.c_str());
	}

	void Log::reportError(const char* format, ...)
	{
		if (!reportErrorFunc || hasReportError)
			return;

		va_list vl;
		va_start(vl, format);
		reportErrorInternal(nullptr, 0, nullptr, format, vl);
		va_end(vl);
	}

	void Log::reportErrorWithFile(const char* filePath, const char* format, ...)
	{
		if (!reportErrorFunc || hasReportError)
			return;

		String content;
		FileResourceManager::Instance()->ReadFileAsText(filePath, content);
		auto fileName = PathUtil::GetPureFilename(filePath);

		va_list vl;
		va_start(vl, format);
		reportErrorInternal(content.data(), content.size(), fileName.c_str(), format, vl);
		va_end(vl);
	}

	void Log::reportErrorWithFile(const char* content, int length, const char* fileName, const char* format, ...)
	{
		if (!reportErrorFunc || hasReportError)
			return;

		va_list vl;
		va_start(vl, format);
		reportErrorInternal(content, length, fileName, format, vl);
		va_end(vl);
	}

	void Log::reportErrorWithResource(const char* resourceName, const char* format, ...)
	{
		if (!reportErrorFunc || hasReportError)
			return;

		String content;
		if (auto stream = FileResourceManager::Instance()->OpenResourceByResName(resourceName))
			content = stream->getAsString();
		auto fileName = PathUtil::GetPureFilename(resourceName);

		va_list vl;
		va_start(vl, format);
		reportErrorInternal(content.data(), content.size(), fileName.c_str(), format, vl);
		va_end(vl);
	}

	void Log::reportErrorInternal(const char* fileContent, int fileSize, const char* fileName, const char* format, va_list argp)
	{
		if (!reportErrorFunc || hasReportError)
			return;
		constexpr size_t kBufferSize = 4096;
		char message[kBufferSize];
		memset(message, 0, kBufferSize);
		vsnprintf(message, kBufferSize - 1, format, argp);
		hasReportError = true;
		reportErrorFunc(message, fileContent, fileSize, fileName);
	}
}

