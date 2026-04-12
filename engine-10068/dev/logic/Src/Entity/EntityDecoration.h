#ifndef __ENTITY_DECORATION_HEADER__
#define __ENTITY_DECORATION_HEADER__
#include "Entity.h"
#include "World/World.h"
namespace BLOCKMAN {

	class EntityDecoration : public Entity
	{
	public:
		RTTI_DECLARE(EntityDecoration);

	protected:
		bool m_isPlayAction = false;
		bool m_canCollided = true;
		bool m_suspended = false;
		i32 m_decorationId = 0;
		String m_name = "";
		String m_actorName = "";
		String m_nameLang = "";
		String m_actorBody = "";
		String m_actorBodyId = "";
		String m_actorAction = "";
		String m_effectName = "";
		
		i32 m_playerActionId = 0;

	private:
		i32 m_oldYaw = 0;
		Vector3 prePosition = Vector3::INVALID;
		map<String, ui64>::type  m_nodes;

	public:
		ui64 m_userId = 0;
		bool m_isActorChange = true;

	public:
		EntityDecoration(World* pWorld);
		EntityDecoration(World* pWorld, const Vector3& pos);
		~EntityDecoration();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}
		void onUpdate() override;
		bool canBeCollidedWith() { return m_canCollided; }
		bool canBeCollidedWithUser(ui64 userId) override;

		void setDecorationId(i32 id);
		i32 getDecorationId() { return m_decorationId; }

		void setActorName(const String& actorName);
		String getActorName() const { return m_actorName; }
		
		void setNameLang(const String& nameLang) { m_nameLang = nameLang; }
		String getNameLang() const { return m_nameLang; }
		String getName() const { return m_name; }
		void setName(const String& name) { m_name = name; }
		void setEffectName(const String & effectName) { m_effectName = effectName; }

		void setActorBody(String body);
		void setActorBodyId(String bodyId);
		void setActorAction(String action);
		String getActorBody() { return m_actorBody; }
		String getActorAction();
		String getActorBodyId() { return m_actorBodyId; }
		String getEffectName() { return m_effectName; }
		i32 getActionId() { return m_playerActionId; }
		bool isImmuneToExplosions() override{ return true; }
		void setCanCollided(bool canCollided) { m_canCollided = canCollided; }

		void setSuspended(bool sign, Vector3 pos) { m_suspended = sign; setPosition(pos); }
		bool getSuspended() { return m_suspended; }

		bool playerJoin(const String& bone , EntityPlayer* player);
		bool playerLeave(ui64 userId);
		const map<String, ui64>::type getNodes() { return m_nodes; }
		void changeNode(const String & node, ui64 userId);
		void setDead() override;
		bool changeAction();
		bool isPlayAction() { return m_isPlayAction; }
		void setPlayAction(bool isPlayAction);
		bool isCanPlayAction();
		bool hasUser();
		
	};
}
#endif
