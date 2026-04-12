#include "EntityBulletin.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Setting/BulletinSetting.h"

namespace BLOCKMAN
{

	EntityBulletin::EntityBulletin(World* pWorld) : EntitySessionNpc(pWorld) 
	{
		m_sessionType = SessionType::NONE;
		preventEntitySpawning = true;
	}

	EntityBulletin::EntityBulletin(World* pWorld, const Vector3& pos) : EntityBulletin(pWorld) 
	{ 
		m_sessionType = SessionType::NONE;
		setPosition(pos); 
	}

	EntityBulletin::~EntityBulletin()
	{
	}

	void EntityBulletin::onUpdate()
	{
		EntitySessionNpc::onUpdate();

		if (!world->m_isClient)
		{
			m_frame++;
			if (m_frame % (20 * 60 * 3) == 0 && m_isPerson)
			{
				removeInexistencePlayerData();
			}
		}
	}

	void EntityBulletin::setRankByUserId(ui64 userId, const BulletinRank & rank)
	{
		if (userId == 0)
		{
			m_userRank = rank;
		}
		m_userRankMap[userId] = rank;
	}

	const BulletinRank & EntityBulletin::getRankByUserId(ui64 userId)
	{
		auto item = m_userRankMap.find(userId);
		if (item != m_userRankMap.end())
		{
			return item->second;
		}
		return m_userRank;
	}

	void EntityBulletin::setBulletinId(i32 bulletinId)
	{
		m_bulletinId = bulletinId;
		const auto bulletin = BulletinSetting::findBulletinById(m_bulletinId);
		if (bulletin)
		{
			//setNameLang(bulletin->name);
			setActorName(bulletin->actorName);
			setActorBody(bulletin->actorBody);
			setActorBodyId(bulletin->actorBodyId);
			setCanBeCollidedWith(bulletin->isCanBeCollided);
		}
	}

	void EntityBulletin::removeInexistencePlayerData()
	{
		vector<i64>::type userList = vector<i64>::type();

		for (auto& item : m_userRankMap)
		{
			if (item.first != 0 && !world->getPlayerByPlatformUserId(item.first))
			{
				userList.push_back(item.first);
			}
		}

		for (auto userId : userList)
		{
			if (m_userRankMap.find(userId) != m_userRankMap.end())
			{
				m_userRankMap.erase(m_userRankMap.find(userId));
			}
		}
	}
}