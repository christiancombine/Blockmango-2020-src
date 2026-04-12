#include "EntityRenderBulletin.h"
#include "Entity/EntityBulletin.h"
#include "EntityRenders/EntityRenderManager.h"
#include "cWorld/Blockman.h"
#include "Actor/ActorObject.h"
#include "Render/HeadTextRenderer.h"
#include "Render/ActorTextRenderer.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Render/RenderEntity.h"
#include "Setting/BulletinSetting.h"
#include "Util/UICommon.h"

namespace BLOCKMAN
{
	EntityRenderBulletin::EntityRenderBulletin()
	{
	}

	void EntityRenderBulletin::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		auto entityBulletin = dynamic_cast<EntityBulletin*>(pEntity);
		if (!entityBulletin)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(entityBulletin);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Vector3 position = pos;
		position.y -= entityBulletin->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);

		int brightUV = entityBulletin->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
		if (entityBulletin->m_isActorChange && entityBulletin->getActorBody().length() > 0 && entityBulletin->getActorBodyId().length() > 0)
		{
			actor->AttachSlave(entityBulletin->getActorBody(), entityBulletin->getActorBodyId());
			entityBulletin->m_isActorChange = false;
		}

		position.y += 2.2f;
		String name = entityBulletin->getName();

		if (name.length() == 0)
		{
			if (entityBulletin->getNameLang().length() > 0)
			{
				name = LanguageManager::Instance()->getString(entityBulletin->getNameLang());
			}
			else
			{
				name = LanguageManager::Instance()->getString(LanguageManager::Instance()->getString(LanguageKey::DEFAULT_SEESION_NAME));
			}
		}

		auto bulletin = BulletinSetting::findBulletinById(entityBulletin->getBulletinId());
		if (bulletin)
		{
			Vector3 titlePos;
			switch ((int)yaw)
			{
			case 0:
				titlePos = Vector3(position.x, pos.y + bulletin->height * (1.f - bulletin->offsetTop - bulletin->lineHeight), position.z - bulletin->width / 2 + bulletin->width * bulletin->offsetWidth);
				break;
			case 90:
				titlePos = Vector3(position.x + bulletin->width / 2 - bulletin->width * bulletin->offsetWidth, pos.y + bulletin->height * (1.f - bulletin->offsetTop - bulletin->lineHeight), position.z);
				break;
			case 180:
				titlePos = Vector3(position.x, pos.y + bulletin->height * (1.f - bulletin->offsetTop - bulletin->lineHeight), position.z + bulletin->width / 2 - bulletin->width * bulletin->offsetWidth);
				break;
			case 270:
				titlePos = Vector3(position.x - bulletin->width / 2 + bulletin->width * bulletin->offsetWidth, pos.y + bulletin->height * (1.f - bulletin->offsetTop - bulletin->lineHeight), position.z);
				break;
			}
			ActorTextRenderer::renderActorText(name, titlePos, true, TextAlign::ENTRE, bulletin->width * bulletin->lineWidth, bulletin->height * bulletin->lineHeight, 1.0f, yaw + 180.f);
			for (int i = 0; i < (int)entityBulletin->getRanks().size(); i++)
			{
				Vector3 textPos;
				switch ((int)yaw)
				{
				case 0:
					textPos = Vector3(position.x, pos.y + bulletin->height * (1.f - bulletin->offsetTop - bulletin->lineSpace * (i + 1) - bulletin->lineHeight * (i + 2)), position.z - bulletin->width / 2 + bulletin->width * bulletin->offsetWidth);
					break;
				case 90:
					textPos = Vector3(position.x + bulletin->width / 2 - bulletin->width * bulletin->offsetWidth, pos.y + bulletin->height * (1.f - bulletin->offsetTop - bulletin->lineSpace * (i + 1) - bulletin->lineHeight * (i + 2)), position.z);
					break;
				case 180:
					textPos = Vector3(position.x, pos.y + bulletin->height * (1.f - bulletin->offsetTop - bulletin->lineSpace * (i + 1) - bulletin->lineHeight * (i + 2)), position.z + bulletin->width / 2 - bulletin->width * bulletin->offsetWidth);
					break;
				case 270:
					textPos = Vector3(position.x - bulletin->width / 2 + bulletin->width * bulletin->offsetWidth, pos.y + bulletin->height * (1.f - bulletin->offsetTop - bulletin->lineSpace * (i + 1) - bulletin->lineHeight * (i + 2)), position.z);
					break;
				}

				String title = StringUtil::Format("%d:%s", entityBulletin->getRanks()[i].rank, entityBulletin->getRanks()[i].name.c_str());
				String score = UICommon::numberToString(entityBulletin->getRanks()[i].score);

				ActorTextRenderer::renderActorText(title, textPos, true, TextAlign::Left, bulletin->width * bulletin->lineWidth, bulletin->height * bulletin->lineHeight, 1.0f, yaw + 180.f);
				ActorTextRenderer::renderActorText(score, textPos, true, TextAlign::RIGHT, bulletin->width * bulletin->lineWidth, bulletin->height * bulletin->lineHeight, 1.0f, yaw + 180.f);
			}
		}
	}

}

