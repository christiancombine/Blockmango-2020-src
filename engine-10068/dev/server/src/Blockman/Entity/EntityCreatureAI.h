#pragma once

#include "Entity/EntityCreature.h"
#include "Behaviac/behaviac_generated/types/behaviac_types.h"

namespace BLOCKMAN
{
	class EntityCreatureAI : public EntityCreature
	{
		RTTI_DECLARE(EntityCreatureAI);

	public:
		EntityCreatureAI(World* pWorld, int monsterId, Vector3& pos, float yaw, String actorName);
		virtual ~EntityCreatureAI();

		/** implement override functions from EntityCreature */
		virtual void onUpdate();
		virtual void onLivingUpdate();
		virtual void    playDeathEffect();
		/** Sets the x,y,z of the entity from the given parameters. Also seems to set up a bounding box. */
		virtual void setPosition(const Vector3& pos) override;

		bool    attackEntityFrom(DamageSource* pSource, float amount);
		void	selectBtTreeByMonsterId(int monsterId);
		void	setCurrentAI(const char* pBtTree);
		void	setAIActive(bool bActive);
		std::string	  getLoadBTTree() { return  m_sLoadBTTree; }
		int     getCreatureAIType() { return m_monsterType; }
		bool    changeCreatureAction(CreatureActionState nActionStatus);
		void	onAttackPlayer(EntityPlayer* player);
		void    attackPerformance(EntityLivingBase* targeter);
		void    groupAttackPerformance();
		void    sendSkillEffect(int effectId);

		void	setEntityHealth(float amount);

		void unloadAgent();

		//����ӿڵ������Ҫ��������Ҫ�õ����ĸ�entity����������attackEntityFrom�����ˡ�attackEntityFrom����ӿ�
		//����������Ϣ������׼ȷ���������ڿ������������� ��֪�����˻��ʲô���⡣��ʱûʱ��ȥ��������
		void notifyAttackFrom(EntityPlayer* pEntity);

	protected:
		virtual bool loadAgent();

		BaseAgent * m_pAgentPlayer = NULL;
		std::string		  m_sLoadBTTree = "";
		int				  m_monsterType = 0;
		int				  m_btTreeId = 0;
	};
}