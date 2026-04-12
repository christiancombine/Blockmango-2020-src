#include "Core.h"
#include "LogConfig.h"
#include "Object/Root.h"

// Keywords
#define _LOGCFG_APPENDER				"APPENDER"
#define _LOGCFG_APPENDER_FILE			"FILE"
#define _LOGCFG_APPENDER_CONSOLE		"CONSOLE"
#define _LOGCFG_APPENDER_VSCONSOLE		"VSCONSOLE"
#define _LOGCFG_LOGFILE					"LOGFILE"
#define _LOGCFG_LOGLEVEL				"LOGLEVEL"
#define _LOGCFG_LOGDISABLE				"LOGDISABLE"
#define _LOGCFG_LOGLEVEL_DEBUG			"DEBUG"
#define _LOGCFG_LOGLEVEL_INFO			"INFO"
#define _LOGCFG_LOGLEVEL_WARNING		"WARNING"
#define _LOGCFG_LOGLEVEL_ERROR			"ERROR"
#define _LOGCFG_LOGLEVEL_FATAL			"FATAL"
#define _LOGCFG_TIMESTAMP				"TIMESTAMP"
#define _LOGCFG_LOGAPPEND				"LOGAPPEND"

namespace LORD
{
	LEVELS LogConfig::getLogLevel(const String& value)
	{
		LEVELS logLevel = G3LOG_INFO;
		if (value == _LOGCFG_LOGLEVEL_DEBUG)
			logLevel = G3LOG_DEBUG;
		else if (value == _LOGCFG_LOGLEVEL_INFO)
			logLevel = G3LOG_INFO;
		else if (value == _LOGCFG_LOGLEVEL_WARNING)
			logLevel = G3LOG_WARNING;
		else if (value == _LOGCFG_LOGLEVEL_ERROR)
			logLevel = G3LOG_ERROR;
		else if (value == _LOGCFG_LOGLEVEL_FATAL)
			logLevel = G3LOG_FATAL;
		return logLevel;
	}

	LogConfig LogConfig::readConfig(bool forceLogAppend, bool isClientMode, const String& logCfgFile)
	{
		LogConfig logConfig;
#if(LORD_PLATFORM != LORD_PLATFORM_WINDOWS)
		logConfig = createDefaultConfig(isClientMode);

#else
		logConfig = createDefaultConfig(isClientMode);
		IFStream fin;
		fin.open(logCfgFile.c_str());
		if (fin.fail())
		{
			return logConfig;
		}
		readConfigFromFile(logCfgFile, fin, logConfig);
		fin.close();
#endif
		if (forceLogAppend && isClientMode) {
#if(LORD_PLATFORM == LORD_PLATFORM_ANDROID)
			logConfig.bLogAppend = true;
#endif
		}
		return logConfig;
	}

	void LogConfig::readConfigFromFile(const String& logCfgFileName, IFStream& fin, LogConfig& config)
	{
		char content[256];
		String contentStr;
		String key;
		String value;
		while (fin.getline(content, 256))
		{
			contentStr = content;
			if (!contentStr.empty() && contentStr[0] == '#')		// comment
				continue;

			size_t pos = contentStr.find('=');
			if (pos != String::npos)
			{
				key = contentStr.substr(0, pos);
				StringUtil::Trim(key);
				StringUtil::UpperCase(key);

				value = contentStr.substr(pos + 1);
				StringUtil::Trim(value);
				StringUtil::UpperCase(value);

				// config appender
				if (key == _LOGCFG_APPENDER)
				{
					StringArray values = StringUtil::Split(value, ",");
					StringArray::iterator valuesIt = values.begin();
					for (; valuesIt != values.end(); ++valuesIt)
					{
						String tempValue = *valuesIt;
						StringUtil::Trim(tempValue);

						if (tempValue == _LOGCFG_APPENDER_CONSOLE)
						{
							config.bConsoleOutput = true;
						}
						else if (tempValue == _LOGCFG_APPENDER_VSCONSOLE)
						{
							config.bVSOutput = true;
						}
						else if (tempValue == _LOGCFG_APPENDER_FILE)
						{
							config.bFileOutput = true;
						}
					}
				}
				else if (key == _LOGCFG_LOGFILE)			// config the log file
				{
					if (!value.empty())
						config.logFilename = value;
				}
				else if (key == _LOGCFG_LOGLEVEL)			// config the log level
				{
					config.logLevel = getLogLevel(value);
				}
				else if (key == _LOGCFG_LOGDISABLE)
				{
					std::vector<std::string> result;
					StringUtil::SplitString(result, value.c_str());
					for (const auto& str : result)
					{
						config.logDisable.emplace_back(getLogLevel(str.c_str()));
					}
				}
				else if (key == _LOGCFG_TIMESTAMP)			// config the time stamp
				{
					if (value == "TRUE")
						config.bTimeStamp = true;
					else
						config.bTimeStamp = false;
				}
				else if (key == _LOGCFG_LOGAPPEND)			// check if append log to the end of file 
				{
					if (value == "TRUE")
						config.bLogAppend = true;
					else
						config.bLogAppend = false;
				}
			}
		} // while
	}

	LogConfig LogConfig::createDefaultConfig(bool isClientMode)
	{
		LogConfig logConfig;
		logConfig.logName = "Main";
		if (isClientMode) {
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
			// support more than one client log exist at the same time 
			logConfig.logFilename = getLogName("client");
#else
			logConfig.logFilename = "client.log";
#endif
		}
		else {
			logConfig.logFilename = "server.log";
		}

#if (LORD_COMPILER == LORD_COMPILER_MSVC)
		logConfig.bConsoleOutput = false;
		logConfig.bVSOutput = true;
#else
#if (LORD_PLATFORM == LORD_PLATFORM_LINUX)
		logConfig.bConsoleOutput = false;
#else
		logConfig.bConsoleOutput = true;
#endif

		logConfig.bVSOutput = false;
#endif

		logConfig.bFileOutput = true;
		logConfig.logLevel = G3LOG_DEBUG;
		logConfig.bTimeStamp = true;
		logConfig.bLogAppend = false;

		return logConfig;
	}

	String LogConfig::getLogName(const String& fileNamePrefix)
	{
		time_t rawtime;
		time(&rawtime);
		tm* timeinfo = localtime(&rawtime);

		char logName[512] = { 0 };
		memset(logName, 0, 512);
		snprintf(logName, 512, "%s_%d_%d_%d_%d_%d.txt", fileNamePrefix.c_str(), timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

		return logName;
	}
}
