#include "BulletinSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, Bulletin*>  BulletinSetting::m_bulletinMap = {};

	bool BulletinSetting::loadSetting(bool bIsClient, bool isMap)
	{
		if (!isMap)
		{
			unloadSetting();
		}

		Bulletin* pBulletin = NULL;
		CsvReader* pReader = new CsvReader();
		bool bRet = pReader->LoadFile(getPath(bIsClient, isMap).c_str());
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			i32 id = 0;
			pReader->GetInt("Id", id);
			pBulletin = findBulletinById(id);
			if (!pBulletin)
			{
				pBulletin = new Bulletin();
				pBulletin->id = id;
			}
			pReader->GetInt("LineNum", pBulletin->lineNum);
			pReader->GetString("Name", pBulletin->name);
			pReader->GetString("ActorName", pBulletin->actorName);
			pReader->GetString("ActorBody", pBulletin->actorBody);
			pReader->GetString("ActorBodyId", pBulletin->actorBodyId);
			pReader->GetString("AwaitAnimate", pBulletin->awaitAnimate);
			pReader->GetFloat("Length", pBulletin->length);
			pReader->GetFloat("Width", pBulletin->width);
			pReader->GetFloat("Height", pBulletin->height);
			pReader->GetFloat("OffsetTop", pBulletin->offsetTop);
			pReader->GetFloat("OffsetWidth", pBulletin->offsetWidth);
			pReader->GetFloat("LineSpace", pBulletin->lineSpace);
			pReader->GetFloat("LineHeight", pBulletin->lineHeight);
			pReader->GetFloat("LineWidth", pBulletin->lineWidth);
			pReader->GetBool("IsCanBeCollided", pBulletin->isCanBeCollided);
			m_bulletinMap[pBulletin->id] = pBulletin;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void  BulletinSetting::unloadSetting()
	{
		for (auto iter = m_bulletinMap.begin(); iter != m_bulletinMap.end(); iter++)
		{
			if (iter->second)
			{
				delete iter->second;
				iter->second = nullptr;
			}
		}

		m_bulletinMap.clear();
	}

	Bulletin * BulletinSetting::findBulletinById(int id)
	{
		auto item = m_bulletinMap.find(id);
		if (item != m_bulletinMap.end())
		{
			return item->second;
		}
		return nullptr;
	}

	String BulletinSetting::getPath(bool bIsClient, bool isMap)
	{
		String path = "";
		if (bIsClient)
		{
			if (isMap)
			{
				path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting/Bulletin.csv").c_str();
			}
			else
			{
				path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/Bulletin.csv").c_str();
			}
		}
		else
		{
			if (isMap)
			{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting/Bulletin.csv").c_str();
#else
				path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting/Bulletin.csv").c_str();
#endif
			}
			else
			{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/Bulletin.csv").c_str();
#else
				path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Bulletin.csv").c_str();
#endif
			}
		}
		return path;
	}

}


