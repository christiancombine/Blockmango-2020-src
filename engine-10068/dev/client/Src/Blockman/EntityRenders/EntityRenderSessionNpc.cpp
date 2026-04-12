#include "EntityRenderSessionNpc.h"
#include "Entity/EntitySessionNpc.h"
#include "EntityRenders/EntityRenderManager.h"
#include "cWorld/Blockman.h"
#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Actor/ActorTemplateData.h"
#include "Anim/AnimBlender.h"
#include "Anim/Skeleton.h"
#include "Anim/Bone.h"
#include "Render/RenderEntity.h"
#include "Render/HeadTextRenderer.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Util/UICommon.h"
#include "Actor/ActorMeshTexModify.h"
#include "EntityRenderBiped.h"
#include "cEntity/EntityPlayerSPMP.h"

namespace BLOCKMAN
{
	EntityRenderSessionNpc::EntityRenderSessionNpc()
	{
	}

	void EntityRenderSessionNpc::renderBloodStrip(EntitySessionNpc * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		if (pEntity->getMaxProcess() <= 0)
			return;
		Vector3 position = pEntity->getPosition(rdt, true);
		position.y += pEntity->getHeight() * pEntity->getScale() - 0.5F - pEntity->yOffset;
		renderEntityBloodStrip(pEntity, position, rdt, pEntity->getCurProcess(), pEntity->getMaxProcess());
	}

	void EntityRenderSessionNpc::renderBloodStripProgress(EntitySessionNpc * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		if (pEntity->getMaxProcess() <= 0)
			return;
		EntityPlayerSPMP* selfPlayer = Blockman::Instance()->m_pPlayer;
		if (selfPlayer == nullptr)
			return;
		float playerPitch = selfPlayer->rotationPitch * Math::DEG2RAD;
		float playerYaw = selfPlayer->rotationYaw * Math::DEG2RAD;
		Vector3 position = pEntity->getPosition(rdt, true);
		position.y += pEntity->getHeight() - 0.4F - pEntity->yOffset;
		i32 cur_num = pEntity->getCurProcess();
		i32 max_num = pEntity->getMaxProcess();
		String progressText = "";
		if (cur_num == max_num)
		{
			progressText = StringUtil::Format(LanguageManager::Instance()->getString("gui_skyblock_progress_full").c_str(), cur_num, max_num);
		} 
		else
		{
			progressText = StringUtil::Format(LanguageManager::Instance()->getString("gui_skyblock_progress_not_full").c_str(), cur_num, max_num);
		}
		HeadTextRenderer::renderHeadText(progressText.c_str(), position, true, 0.5f);
	}

	void EntityRenderSessionNpc::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		auto entitySession = dynamic_cast<EntitySessionNpc*>(pEntity);
		if (!entitySession)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(entitySession);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Vector3 position = pos;
		position.y -= entitySession->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);

		int brightUV = entitySession->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
		if (entitySession->m_isActorChange && entitySession->getActorBody().length() >0 && entitySession->getActorBodyId().length() > 0)
		{
			actor->AttachSlave(entitySession->getActorBody(), entitySession->getActorBodyId());
			entitySession->m_isActorChange = false;
			if (entitySession->getActorAction().length() > 0)
			{
				actor->PlaySkill(entitySession->getActorAction());
			}
			//actor->DelMeshTexModify();
			//if (entitySession->getActorBody() == "rank")
			//{
			//	actor->AddMeshTexModify(entitySession->getActorBody(), entitySession->getActorBodyId(), Vector2i(0, 5), Vector2i(174, 18));
			//}
			if (entitySession->getActorBody() == "bagpack")
			{
				actor->DelMeshTexModify();
				actor->AddMeshTexModify(entitySession->getActorBody(), entitySession->getActorBodyId(), Vector2i(1, 214), Vector2i(252, 40));
				actor->getMeshTexModify()->updateString(0.5f, "");
				actor->getMeshTexModify()->updateString(0.f, "");
			}
		}

		if (entitySession->getEffectName().length() > 0)
		{
			String effectName = entitySession->getEffectName();
			actor->AttachSelectEffect(effectName);
		}

		position.y += entitySession->getHeight() + 0.2f;
		String name = entitySession->getName();
		if (name.length() == 0)
		{
			if (entitySession->getNameLang().length() > 0)
			{
				name = LanguageManager::Instance()->getString(entitySession->getNameLang());
			}
		}

		if (entitySession->getTimeLeft() > 1000)
		{
			switch (entitySession->getSessionType())
			{
			case  SessionType::TREASURE_CHEST:
				name = StringUtil::Format(LanguageManager::Instance()->getString("gui_bird_next_refresh_chest").c_str(), UICommon::timeFormat(entitySession->getTimeLeft()).c_str());
				break;
			case  SessionType::BIRD_TASK_TIP:
				name = StringUtil::Format(LanguageManager::Instance()->getString("gui_bird_next_task_time").c_str(), UICommon::timeFormat(entitySession->getTimeLeft()).c_str());
				break;
			case  SessionType::BIRD_EGG_CHEST:
				name = StringUtil::Format(LanguageManager::Instance()->getString("gui_bird_next_refresh_chest").c_str(), UICommon::timeFormat(entitySession->getTimeLeft()).c_str());
				break;
			case  SessionType::BLOCK_CITY_TIGER_LOTTERY:
				name = StringUtil::Format(LanguageManager::Instance()->getString("gui_blockcity_next_refersh_tigerlottery").c_str(), UICommon::timeFormat(entitySession->getTimeLeft()).c_str());
				break;
			case SessionType::SKY_WAR_LUCKY_BLOCK_BOX:
				name = UICommon::luckyBlockTimeFormat(entitySession->getTimeLeft());
				break;
			//default:
				//name = UICommon::timeFormat(entitySession->getTimeLeft());
			//	break;
			}
		}

		if (entitySession->getInfoTime() > 0)
		{
			if (entitySession->getSessionType() == SessionType::SKYBLOCK_PRODUCT_NPC)
			{
				String nameLang = entitySession->getStringMsg().c_str();
				entitySession->setNameLangByUser(0, nameLang);
				int seconds = entitySession->getInfoTime() / 1000;
				int second = seconds % 60;
				int minute = (seconds % 3600) / 60 + 1;
				int hour = seconds / 3600;
				if (minute == 60)
				{
					hour += 1;
					minute = 0;
				}
				name = StringUtil::Format(LanguageManager::Instance()->getString(entitySession->getStringMsg().c_str()).c_str(), entitySession->getInfoLv(), hour > 0 ? hour : 0, minute > 0 && minute < 60 ? minute : 0);
				entitySession->setActorAction("run");
			}
			else if (entitySession->getSessionType() == SessionType::SKY_BLOCK_CHRISTMAS)
			{
				String nameLang = entitySession->getStringMsg().c_str();
				entitySession->setNameLangByUser(0, nameLang);
				int time = entitySession->getInfoTime();
				if (time > 0)
				{
					int times = time / 1000;
					int hour = times / 3600;
					int minutes = (times % 3600) / 60;
					int seconds = times % 3600 % 60;
					name = StringUtil::Format(LanguageManager::Instance()->getString(entitySession->getStringMsg().c_str()).c_str(), hour, minutes, seconds);
				}
			}
			else
			{
				name = StringUtil::Format(LanguageManager::Instance()->getString(entitySession->getStringMsg().c_str()).c_str(), entitySession->getInfoLv(), entitySession->getInfoTime() / 1000);
			}
		}
		else 
		{
			if (entitySession->getSessionType() == SessionType::SKYBLOCK_PRODUCT_NPC)
			{
				String nameLang = entitySession->getStringMsg().c_str();
				entitySession->setNameLangByUser(0, nameLang);
				name = StringUtil::Format(LanguageManager::Instance()->getString(entitySession->getStringMsg().c_str()).c_str(), entitySession->getInfoLv());
				String action = entitySession->getInfoLv() ==  0 ? "idle" : "run" ;
				entitySession->setActorAction(action);
			}
			else if (entitySession->getSessionType() == SessionType::SKY_BLOCK_CHRISTMAS)
			{
				name = StringUtil::Format(LanguageManager::Instance()->getString("gui_sky_block_christmas_reward").c_str());
			}

		}

		if (entitySession->getSessionType() == SessionType::SKYBLOCK_PRODUCT_NPC)
		{
			renderBloodStrip(entitySession, pos, yaw, rdt);
			renderBloodStripProgress(entitySession, pos, yaw, rdt);
		}

		float scale = 1.0f;
		if (entitySession->getSessionType() == SessionType::BIRD_TREE)
		{
			scale = 5.0f;
		}

		HeadTextRenderer::renderHeadText(name.c_str(), position, true, scale);
	}

}

