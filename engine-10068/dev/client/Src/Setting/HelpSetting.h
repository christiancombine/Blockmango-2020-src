#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	class  HelpItem {
	public:
		int id = 0;
		String icon = "";
		String name = "";
		String content = "";
	};

	class HelpSetting
	{
	public:
		static bool loadSetting();
		static void unloadSetting();

	public:
		static const  vector<HelpItem*>::type& getHelps();

	private:
		static vector<HelpItem*>::type m_Helps;
	};
}
