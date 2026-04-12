#include "EntityRenderCreature.h"
#include "EntityRenderManager.h"
#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Render/HeadTextRenderer.h"
#include "Model/ModelBiped.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "Render/RenderEntity.h"
#include "Util/PlayerPrefs.h"

namespace BLOCKMAN
{

	EntityRenderCreature::EntityRenderCreature()
		: EntityRenderLiving(LordNew ModelBiped(0.f, 0.f), 0.5f)
	{
	}

	EntityRenderCreature::~EntityRenderCreature()
	{
	}

	void EntityRenderCreature::renderCreature_impl(EntityCreature * pCreature, const Vector3 & pos, float yaw, float rdt)
	{
		if (!pCreature)
		{
			return;
		}
		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(pCreature);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Color color(1.f, 1.f, 1.f, 1.f);
		if (pCreature->isHurt())
		{
			color.set(1.f, 0.4f, 0.f, 1.f);
		}
		actor->SetNaturalColor(color, true);
		Vector3 position = pCreature->prevPos + (pCreature->position - pCreature->prevPos) * rdt;
		position.y -= pCreature->yOffset;
		actor->SetPosition(position);

		if (pCreature->getScale() != 1.0f)
			actor->SetScale({ pCreature->getScale(), pCreature->getScale(), pCreature->getScale() });
		
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);
		int brightUV = pCreature->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
		actor->AttachSlave("body", StringUtil::ToString(1));
		playCreatureAction(pCreature, actor);
		position.y += pCreature->getHeight() + 0.2f;
		String name = pCreature->getNameLang();
		if (name.length() > 0)
		{
			if (StringUtil::Find(name, "="))
			{
				StringArray kvPair = StringUtil::Split(name, "=");
				name = StringUtil::Format(LanguageManager::Instance()->getString(kvPair[0]).c_str(), kvPair[1].c_str());
			}
			else
			{
				name = LanguageManager::Instance()->getString(name);
			}
			HeadTextRenderer::renderHeadText(name.c_str(), position, true);
		}

		/*float fScale = pCreature->getScale();
		actor->SetScale(Vector3(fScale, fScale, fScale));*/

		renderBloodStrip(pCreature, pos, yaw, rdt);
	}

	void EntityRenderCreature::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float ticks)
	{
		renderCreature_impl((EntityCreature*)pEntity, pos, yaw, ticks);
	}

	bool EntityRenderCreature::compileSkinMesh(SkinedRenderable * pRenderable, const Vector2i & pos, const Vector2i & size)
	{
		return false;
	}

	bool EntityRenderCreature::playCreatureAction(EntityCreature * pCreature, ActorObject* actor)
	{
		playBaseAction(pCreature, actor);
		playUpperAction(pCreature, actor);
		return true;
	}

	void EntityRenderCreature::playBaseAction(EntityCreature* pCreature, ActorObject* actor)
	{
		CreatureActionState baseAction = pCreature->getBaseAction();
		CreatureActionState prevBaseAction = pCreature->getPrevBaseAction();

		if (prevBaseAction == baseAction)
			return;

		String skillName = getSkillName(pCreature, baseAction);
		if (!skillName.empty())
			actor->PlaySkill(skillName);

		pCreature->refreshBaseAction();
		pCreature->setUpperAction(baseAction);
	}

	void EntityRenderCreature::playUpperAction(EntityCreature* pCreature, ActorObject* actor)
	{
		CreatureActionState prevUpperAction = pCreature->getPrevUpperAction();
		CreatureActionState upperAction = pCreature->getUpperAction();
		CreatureActionState baseAction = pCreature->getBaseAction();

		if (prevUpperAction == upperAction)
			return;

		String skillName = getSkillName(pCreature, baseAction);
		if (!skillName.empty())
		{
			actor->PlaySkill(skillName);
		}

		pCreature->refreshUpperAction();
	}

	String  EntityRenderCreature::getSkillName(EntityCreature* pCreature, CreatureActionState nActionState)
	{
		String  SkillName = "idle";
		switch (nActionState)
		{
		case CREATURE_AS_ATTACK:
			if(pCreature->isBoss())
				SkillName = "attack2";
			else if (pCreature->isMonster())
				SkillName = "attack";
			break;
		case CREATURE_AS_BE_OTHER_ATTACK:
			break;
		case CREATURE_AS_MOVE:
			SkillName = "walk";
			break;
		case CREATURE_AS_DEATH:
		    if(pCreature->hasDeathAction())
				SkillName = "hitdown";
			break;
		case CREATURE_AS_RUN:
			if (pCreature->hasRunAction())
				SkillName = "run";
			else
				SkillName = "walk";
			break;
		default:
			break;
		}
		return SkillName;
	}

	void EntityRenderCreature::renderBloodStrip(EntityCreature * pCreature, const Vector3 & pos, float yaw, float rdt)
	{
		if (!PlayerPrefs::Instance()->getBoolPrefs("IsCreatureBloodBar"))
			return;
		Vector3 position = pCreature->getPosition(rdt, true);
		position.y += pCreature->getHeight() * pCreature->getScale() + 1.3F - pCreature->yOffset + pCreature->m_bloodStripOffset;
		renderEntityBloodStrip(pCreature, position, rdt, pCreature->getHealth(), pCreature->getMaxHealth());
	}
}


