#pragma once

#ifndef __LOG_CONFIG_HEADER__
#define __LOG_CONFIG_HEADER__

#include "Memory/LordMemory.h"
#include "g3log/loglevels.hpp"

namespace LORD
{
	class LogConfig
	{
	public:
		// logName is a tag that be put at the beginning of each line of log message
		String logName = "LordLog";

		// logFilename is the name of log file
		String logFilename = "lordlog.txt";

		// whether the log stream flow into console sink
		bool bConsoleOutput = true;

		// whether the log stream flow into visual studio console sink
		bool bVSOutput = false;

		// whether the log stream flow into file sink
		bool bFileOutput = false;

		// enable log level >= logLevel. 
		LEVELS logLevel = G3LOG_INFO;

		// whether put the local timestamp at the beginning of each line of log message
		bool bTimeStamp = true;

		// whether append the log message after the old content of the file, or just truncate it
		bool bLogAppend = false;
		std::vector<LEVELS> logDisable;

		static LEVELS getLogLevel(const String& value);
		static LogConfig readConfig(bool forceLogAppend, bool isClientMode, const String& logCfgFile);

	private:
		static void readConfigFromFile(const String& logCfgFileName, IFStream& fin, LogConfig& config);
		static LogConfig createDefaultConfig(bool isClientMode);
		static String getLogName(const String& fileNamePrefix);
	};
}

#endif // !__LOG_CONFIG_HEADER__
