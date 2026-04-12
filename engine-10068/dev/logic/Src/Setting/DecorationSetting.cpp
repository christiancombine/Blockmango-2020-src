#include "DecorationSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<i32, Decoration*>  DecorationSetting::m_decorationMap = {};

	bool DecorationSetting::loadSetting(bool isClient)
	{
		Decoration* pDecoration = NULL;
		m_decorationMap.clear();
		CsvReader* pReader = new CsvReader();
		std::string path = "";
		if (isClient)
		{
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting/decoration.csv").c_str();
		}
		else
		{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting/decoration.csv").c_str();
#else
			path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), "setting/decoration.csv").c_str();
#endif
		}
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}

		pReader->Read(); // discard the second line

		while (pReader->Read())
		{
			pDecoration = new Decoration();
			pReader->GetInt("Id", pDecoration->id);
			pReader->GetInt("Type", pDecoration->type);
			pReader->GetInt("ItemId", pDecoration->itemId);
			pReader->GetInt("ActionId", pDecoration->actionId);
			pReader->GetInt("InteractionType", pDecoration->interactionType);
			i32 isCanCollided = 0;
			pReader->GetInt("CanCollided", isCanCollided);
			pDecoration->isCanCollided = isCanCollided > 0;
			pReader->GetString("Name", pDecoration->name);
			pReader->GetString("ActorName", pDecoration->actorName);
			pReader->GetString("ActorBody", pDecoration->actorBody);
			pReader->GetString("ActorBodyId", pDecoration->actorBodyId);
			
			pReader->GetString("UseAnimate", pDecoration->useAnimate);
			pReader->GetFloat("Length", pDecoration->length);
			pReader->GetFloat("Width", pDecoration->width);
			pReader->GetFloat("Height", pDecoration->height);
			pReader->GetString("Icon", pDecoration->icon);
			pReader->GetString("MeshName", pDecoration->meshName);
			String nodes;
			pReader->GetString("Nodes", nodes);
			auto arr = StringUtil::Split(nodes, "#");
			for (auto str : arr)
			{
				pDecoration->m_nodes.push_back(str);
			}

			String awaitAnimate;
			pReader->GetString("AwaitAnimate", awaitAnimate);
			auto animateArr = StringUtil::Split(awaitAnimate, "#");
			if (animateArr.size() == 1)
			{
				pDecoration->awaitAnimate = awaitAnimate;
			}
			else
			{
				for (size_t i = 0; i< pDecoration->m_nodes.size(); i++)
				{
					pDecoration->awaitAnimate = animateArr[0];
					pDecoration->awaitAnimates[pDecoration->m_nodes[i]] = animateArr[i];
				}
			}

			m_decorationMap[pDecoration->id] = pDecoration;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void  DecorationSetting::unloadSetting()
	{
		for (auto iter = m_decorationMap.begin(); iter != m_decorationMap.end(); iter++)
		{
			if (iter->second)
			{
				delete iter->second;
				iter->second = nullptr;
			}
		}

		m_decorationMap.clear();
	}

	Decoration * DecorationSetting::findItem(i32 id)
	{
		auto item = m_decorationMap.find(id);
		if (id != 0 && item != m_decorationMap.end())
		{
			return item->second;
		}
		return nullptr;
	}

	Decoration * DecorationSetting::findItemByItemId(i32 itemId)
	{
		for (auto item : m_decorationMap)
		{
			if (item.second->itemId  == itemId)
			{
				return item.second;
			}
		}
		return nullptr;
	}

	bool DecorationSetting::canPlayAction(i32 id)
	{
		if (auto decoration = findItem(id))
		{
			return decoration->interactionType > 1;// || decoration->interactionType == 3;
		}
		return false;
	}

	String DecorationSetting::getAction(i32 id, map<String, ui64>::type nodes)
	{
		if (auto decoration = findItem(id))
		{
			if (decoration->interactionType == 4)
			{
				bool isFull = true;
				String node = "";
				for (auto& item : nodes)
				{
					if (item.second == 0)
					{
						isFull = false;
					}
					else
					{
						node = item.first;
					}
				}

				if (isFull)
				{
					return decoration->useAnimate;
				}
				else
				{
					decoration->awaitAnimate = decoration->awaitAnimates[node];
					return decoration->awaitAnimate;
				}
			}

			return decoration->useAnimate;
		}
		return "";
	}

}
