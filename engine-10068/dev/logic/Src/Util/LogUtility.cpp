#include "LogUtility.h"
#include "Log/LogConfig.h"

void LogUtility::setHighest(const LORD::String & level)
{
	g3::log_levels::setHighest(LORD::LogConfig::getLogLevel(level));
}

void LogUtility::disable(const LORD::String & level)
{
	g3::log_levels::disable(LORD::LogConfig::getLogLevel(level));
}

void LogUtility::enable(const LORD::String & level)
{
	g3::log_levels::enable(LORD::LogConfig::getLogLevel(level));
}

void LogUtility::disableAll()
{
	g3::log_levels::disableAll();
}

void LogUtility::enableAll()
{
	g3::log_levels::enableAll();
}
