#pragma once

#ifndef __THUMBUP_MANAGER_HEADER__
#define __THUMBUP_MANAGER_HEADER__
#include "Core.h"
#include <sstream>
#include "Object/Root.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
using namespace LORD;

namespace BLOCKMAN {

	class ThumbUpRecord
	{
	public:
		i64 messageId = 0;
		i64 targetId = 0;
		String picUrl = "";
		String userName = "";
		ThumbUpRecord() {}
	};

	class ThumbUpRecordManager : public ObjectAlloc
	{
	public:
		static void parseThumbRecordData(const String & data);
	};
}

#endif