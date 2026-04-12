#include "MultiLanTipSetting.h"
#include "Core.h"
#include <sstream>
#include "Object/Root.h"
#include "Util/CSVReader.h"

using namespace LORD;

MultiLanTipSetting::MultiLanTipSettingMap MultiLanTipSetting::ms_multiLanTipSettingMap = {};
std::set<std::string>					  MultiLanTipSetting::ms_languageSet = 
{
	"en_US",			//英文/美国
	"zh_CN",			//中文简体
	"zh_TW",			//中文繁体 
	"de_DE",			//德语
	"nl_BE",			//荷兰语/比利时
	"nl_NL",			//荷兰语/荷兰
	"en_AU",			//英语/澳大利亚
	"en_GB",			//英语/英国
	"en_CA",			//英语/加拿大
	"en_NZ",			//英语/新西兰
	"en_SG",			//英语/新加坡
	"fr_BE",			//法语/比利时
	"fr_CA",			//法语/加拿大
	"fr_FR",			//法语/法国
	"fr_CH",			//法语/瑞士
	"de_AT",			//德语/奥地利
	"de_LI",			//德语/列支登士等
	"de_CH",			//德语/瑞士
	"it_IT",			//意大利语, 意大利
	"it_CH",			//意大利语, 瑞士
	"ja_JP",			//日语
	"ko_KR",			//韩语
	"pl_PL",			//波兰语
	"ru_RU",			//俄语
	"es_ES",			//西班牙语
	"ar_EG",			//阿拉伯语, 埃及
	"ar_IL",			//阿拉伯语, 以色列
	"bg_BG",			//保加利亚语, 保加利亚
	"ca_ES",			//加泰罗尼亚语, 西班牙
	"hr_HR",			//克罗地亚语, 克罗地亚
	"da_DK",			//丹麦语, 丹麦
	"en_IN",			//英语, 印度
	"en_IE",			//英语, 爱尔兰
	"en_ZA",			//英语, 津巴布韦
	"fi_FI",			//芬兰语, 芬兰
	"el_GR",			//希腊语, 希腊
	"iw_IL",			//希伯来语, 以色列
	"hi_IN",			//北印度语, 印度
	"hu_HU",			//匈牙利语, 匈牙利
	"in_ID",			//印度尼西亚语, 印度尼西亚
	"lv_LV",			//拉脱维亚语, 拉脱维亚
	"lt_LT",			//立陶宛语, 立陶宛
	"nb_NO",			//Norwegian-Bokmol, 挪威
	"pt_BR",			//葡萄牙语, 巴西
	"pt_PT",			//葡萄牙语, 葡萄牙
	"ro_RO",			//罗马尼亚语, 罗马尼亚
	"sr_RS",			//塞尔维亚语
	"sk_SK",			//斯洛伐克语, 斯洛伐克
	"sl_SI",			//斯洛维尼亚语, 斯洛文尼亚
	"es_US",			//西班牙语, 美国
	"sv_SE",			//瑞典语, 瑞典
	"tl_PH",			//塔加拉族语, 菲律宾
	"th_TH",			//泰国语, 泰国
	"tr_TR",			//土耳其语, 土耳其
	"uk_UA",			//乌克兰语, 乌克兰
	"vi_VN",			//越南语, 越南
};

MultiLanTipSetting::MultiLanTipSetting()
{

}

MultiLanTipSetting::~MultiLanTipSetting()
{
	m_TipMap.clear();
}

bool MultiLanTipSetting::loadSetting(bool isClient)
{
	unloadSetting();
	String EngineBaseDir = "";
	if (isClient)
	{
		EngineBaseDir = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/GameTip");
	}
	else
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		EngineBaseDir = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/GameTip");
#else
		EngineBaseDir = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "GameTip");
#endif
	}
	LordLogInfo("Load GameTip EngineBaseDir: %s", EngineBaseDir.c_str());
	StringArray gameTipFiles;
	PathUtil::EnumFilesInDir(gameTipFiles, EngineBaseDir, false, false, true);

	for (String path : gameTipFiles)
	{
		LordLogInfo("Load EngineGameTip FilePath: %s", path.c_str());
		std::string filePath = path.c_str();
		loadFileToSetting(filePath);
	}
	return true;
}

bool MultiLanTipSetting::loadMapSetting(bool isClient)
{
	String MapBaseDir = "";
	if (isClient)
	{
		MapBaseDir = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "GameTip");
	}
	else
	{
		MapBaseDir = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "GameTip");
	}
	LordLogInfo("Load GameTip MapBaseDir: %s", MapBaseDir.c_str());
	StringArray gameTipFiles;
	PathUtil::EnumFilesInDir(gameTipFiles, MapBaseDir, false, false, true);
	for (String path : gameTipFiles)
	{
		LordLogInfo("Load MapGameTip FilePath: %s", path.c_str());
		std::string filePath = path.c_str();
		loadFileToSetting(filePath);
	}
	return true;
}

bool MultiLanTipSetting::unloadSetting()
{
	for (auto iter = ms_multiLanTipSettingMap.begin(); iter != ms_multiLanTipSettingMap.end(); iter++)
	{
		MultiLanTipSetting* pSetting = iter->second;
		delete pSetting;
	}

	ms_multiLanTipSettingMap.clear();
	return true;
}

std::string MultiLanTipSetting::getMessage(String& lanType, int nTipType, const char* args)
{
	std::string msg = getMessage(lanType, nTipType);
	return formatMessage(lanType, msg, args);
}

std::string MultiLanTipSetting::getMessageToLua(String& lanType, int nTipType, const char* args)
{
	std::string msg = getMessage(lanType, nTipType);
	return formatMessage(lanType, msg, args);
}

std::string MultiLanTipSetting::formatMessage(String& lanType, std::string& msg, const char* args)
{
	if (!args)
		return msg;

	std::string::size_type		pos;
	unsigned int				index		= 1;
	std::string::size_type		size		= msg.size();
	StringArray					argsList	= StringUtil::Split(String(args), "\t", 0, true); //the args spilts with "\t"
	std::string					pattern;

	while (true)
	{
		if (index > argsList.size())
			break;

		pattern = StringUtil::concatToString("%", index, "s").c_str();
		pos = msg.find(pattern, 0);
		if (pos >= size)
			break;

		std::string arg = argsList[index - 1].c_str();
		std::string argString = getArgString(lanType, arg);
		msg = msg.replace(pos, pattern.size(), argString);
		size = msg.size();
		index++;
	}
	
	return msg;
}

bool MultiLanTipSetting::loadFileToSetting(std::string& path)
{
	CsvReader* pReader = new CsvReader();
	bool bRet = pReader->LoadFile(path);
	if (!bRet)
	{
		LordLogError("Can not open MultiLanTipSetting file %s", path.c_str());
		LordSafeDelete(pReader);
		return false;
	}

	MultiLanTipSetting* pSetting = NULL;
	pReader->Read(); //discard the second line

	while (pReader->Read())
	{
		pSetting = new MultiLanTipSetting();
		pReader->GetInt("TipType", pSetting->m_nTipId);

		std::string tipStr;
		for (auto iter = ms_languageSet.begin(); iter != ms_languageSet.end(); iter++)
		{
			if (pReader->GetString(*iter, tipStr))
			{
				pSetting->m_TipMap[*iter] = tipStr;
			}
		}

		auto oldSetting = ms_multiLanTipSettingMap.find(pSetting->m_nTipId);
		if (oldSetting != ms_multiLanTipSettingMap.end())
		{
			delete oldSetting->second;
			ms_multiLanTipSettingMap.erase(pSetting->m_nTipId);
		}

		ms_multiLanTipSettingMap.insert({ pSetting->m_nTipId, pSetting });
	}
	LordSafeDelete(pReader);
	return true;
}

std::string MultiLanTipSetting::getMessage(String& lanType, int nTipType)
{
	MultiLanTipSetting* pSetting = NULL;

	auto iter = ms_multiLanTipSettingMap.find(nTipType);
	if (iter != ms_multiLanTipSettingMap.end())
	{
		pSetting = iter->second;
	}
	else
	{
		LordLogError("Can not find TipType %d", nTipType);
		return "";
	}

	auto it = pSetting->m_TipMap.find(lanType.c_str());
	if (it == pSetting->m_TipMap.end())
	{
		//LordLogError("Can not find TipType %d LanType %s, return default English!!!", nTipType, lanType.c_str());
		it = pSetting->m_TipMap.find("en_US");
	}

	if (it != pSetting->m_TipMap.end())
		return it->second;
	else
		return "";
}

//for example: replace "#66#" with the template which nTipType == 66
std::string MultiLanTipSetting::getArgString(String& lanType, std::string& arg)
{
	if (arg.size() < 3)
		return arg;

	unsigned int pos1;
	unsigned int pos2;

	std::string substr;
	std::string actualStr;

	while (true)
	{
		pos1 = arg.find("#", 0);
		if (pos1 >= arg.size() - 2)
			return arg;

		pos2 = arg.find("#", pos1 + 2);
		if (pos2 > arg.size())
			return arg;
	
		substr = arg.substr(pos1 + 1, pos2 - pos1 - 1);
		int nTipType = StringUtil::ParseI32(substr.c_str(), -1);

		if (nTipType < 0)
		{
			return arg;
		}

		actualStr = getMessage(lanType, nTipType);
		if (actualStr.size() > 0)
		{
			arg = arg.replace(pos1, pos2 - pos1 + 1, actualStr);
		}
		else
		{
			break;
		}
	}

	return arg;
}

