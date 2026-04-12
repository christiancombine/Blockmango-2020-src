#include "BulletClipSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "Item/Items.h"

namespace BLOCKMAN
{
	std::map<int, BulletClipSetting*> BulletClipSetting::m_bulletClipSettingMap = {};

	bool BulletClipSetting::loadSetting(const String & path)
	{
		BulletClipSetting* pSetting = NULL;
		CsvReader* pReader = new CsvReader();
		
		bool bRet = pReader->LoadFile(path.c_str());
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}

		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pSetting = new BulletClipSetting();
			pReader->GetInt("BulletClipId", pSetting->bulletId);
			pReader->GetInt("MaxStack", pSetting->maxStack);

			pReader->GetString("Name", pSetting->bulletName);
			pReader->GetString("ClipModel", pSetting->bulletClipModel);
			pReader->GetString("BulletIcon", pSetting->bulletIcon);
			pReader->GetString("Icon", pSetting->icon);

			auto oldSetting = m_bulletClipSettingMap[pSetting->bulletId];
			if (oldSetting)
			{
				LordSafeDelete(oldSetting);
				m_bulletClipSettingMap[pSetting->bulletId] = NULL;
			}
			m_bulletClipSettingMap[pSetting->bulletId] = pSetting;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void BulletClipSetting::unloadSetting()
	{
		for (auto iter = m_bulletClipSettingMap.begin(); iter != m_bulletClipSettingMap.end(); iter++)
		{
			BulletClipSetting* pSetting = iter->second;
			delete pSetting;
		}

		m_bulletClipSettingMap.clear();
	}

	BulletClipSetting * BulletClipSetting::getBulletClipSetting(int bulletId)
	{
		auto iter = m_bulletClipSettingMap.find(bulletId);
		if (iter != m_bulletClipSettingMap.end())
		{
			return iter->second;
		}

		return nullptr;
	}

	void BulletClipSetting::updateBulletClipSetting(BulletClipPluginSetting setting)
	{
		BulletClipSetting * bulletClipSetting = getBulletClipSetting(setting.bulletId);
		if (bulletClipSetting)
		{
			bulletClipSetting->maxStack = setting.maxStack;
			ItemBulletClip* pBulletClip = dynamic_cast<ItemBulletClip*>(Item::itemsList[setting.bulletId]);
			if (pBulletClip) {
				pBulletClip->setMaxStackSize(setting.maxStack);
			}
		}
	}
}


