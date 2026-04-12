#ifndef __ENTITY_BULLETIN_HEADER__
#define __ENTITY_BULLETIN_HEADER__

#include "EntitySessionNpc.h"
#include "World/World.h"
#include "Network/protocol/PacketDefine.h"

namespace BLOCKMAN {

	class BulletinRank
	{
	public:
		i32 rank = 0;
		i64 userId = 0;
		i64 score = 0;
		String name = "";

		DEFINE_NETWORK_SERIALIZER(rank, userId, score, name);

		BulletinRank() 
		{
		}
	};

	class EntityBulletin : public EntitySessionNpc
	{
		RTTI_DECLARE(EntityBulletin);
	protected:
		BulletinRank m_userRank;
		vector<BulletinRank>::type m_ranks;
		map<ui64, BulletinRank>::type m_userRankMap;
		String m_awaitAnimate = "idle";
		i32 m_bulletinId = 0;

	public:
		EntityBulletin(World* pWorld);
		EntityBulletin(World* pWorld, const Vector3& pos);
		~EntityBulletin();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}
		void onUpdate() override;
		void setRank(const BulletinRank& rank) { m_userRank = rank; }
		void setRankByUserId(ui64 userId, const BulletinRank& rank);
		void setRanks(const vector<BulletinRank>::type& ranks) { m_ranks = ranks; }
		const vector<BulletinRank>::type& getRanks() { return m_ranks; }
		const BulletinRank& getRankByUserId(ui64 userId);
		const BulletinRank& getRank() { return m_userRank; };

		void setBulletinId(i32 bulletinId);
		i32 getBulletinId() { return m_bulletinId; }
		
		bool isImmuneToExplosions() override{ return true; }
		bool canBeCollidedWith() { return m_canCollided; }
		void setCanBeCollidedWith(bool isCanBeCollided) { m_canCollided = isCanBeCollided; }
	private:
		i64 m_frame = 0;
		void removeInexistencePlayerData();
	};
}
#endif
