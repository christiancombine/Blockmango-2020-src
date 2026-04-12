#pragma once

#ifndef _LOG_UTILITY_H_
#define _LOG_UTILITY_H_

#include "Core.h"

class LogUtility
{
public:
	static void setHighest(const LORD::String& level);

	static void disable(const LORD::String& level);
	static void enable(const LORD::String& level);

	static void disableAll();
	static void enableAll();
};

#endif