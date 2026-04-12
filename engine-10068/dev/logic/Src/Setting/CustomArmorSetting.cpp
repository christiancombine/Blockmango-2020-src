#include "CustomArmorSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "CustomArmorSetting.h"

namespace BLOCKMAN
{
	CustomArmorSetting::CustomArmorSetting()
	{

	}

	CustomArmorSetting::~CustomArmorSetting()
	{

	}

	bool CustomArmorSetting::loadSetting(bool isClient)
	{
		m_armorInfoMap.clear();

		CsvReader* pReader = new CsvReader();

		std::string path;
		if (isClient)
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "CustomArmor.csv").c_str();
		}
		else
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting", "CustomArmor.csv").c_str();
		}
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}
		pReader->Read(); // discard the second line
		pReader->Read();
		while (pReader->Read())
		{
			CustomArmorSettingInfo armorSettingInfo;
			pReader->GetInt("UniqueId", armorSettingInfo.uniqueId);
			pReader->GetInt("ItemId", armorSettingInfo.itemId);
			pReader->GetInt("Attack", armorSettingInfo.attack);
			pReader->GetInt("Defence", armorSettingInfo.defence);
			pReader->GetInt("Hp", armorSettingInfo.hp);
			m_armorInfoMap[armorSettingInfo.uniqueId] = armorSettingInfo;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void CustomArmorSetting::unloadSetting()
	{
		m_armorInfoMap.clear();
	}

	bool CustomArmorSetting::getArmorSettingInfo(int itemId, CustomArmorSettingInfo& armorSettingInfo)
	{
		for (auto iter = m_armorInfoMap.begin(); iter != m_armorInfoMap.end(); ++iter)
		{
			if (iter->second.itemId == itemId)
			{
				armorSettingInfo = iter->second;
				return true;
			}
		}
		return false;
	}

	bool CustomArmorSetting::isCustomArmorByItemId(int itemId)
	{
		for (auto iter = m_armorInfoMap.begin(); iter != m_armorInfoMap.end(); ++iter)
		{
			if (iter->second.itemId == itemId)
			{
				return true;
			}
		}
		return false;
	}
}