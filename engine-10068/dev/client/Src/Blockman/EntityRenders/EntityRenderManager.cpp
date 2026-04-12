#include "EntityRenderManager.h"
#include "EntityRender.h"
#include "EntityRenderPlayer.h"
#include "EntityRenderItem.h"
#include "EntityRenderable.h"
#include "EntityRenderArrow.h"
#include "EntityRenderBullet.h"
#include "EntityRenderThrowable.h"
#include "EntityRenderTNTPrimed.h"
#include "EntityRenderMerchant.h"
#include "EntityRenderVehicle.h"
#include "EntityRenderRankNpc.h"
#include "EntityRenderAircraft.h"
#include "EntityRenderActorNpc.h"
#include "EntityRenderSessionNpc.h"
#include "EntityRenderCreature.h"
#include "EntityRenderCreatureBullet.h"
#include "EntityRenderSkillThrowable.h"
#include "EntityRenderGrenade.h"
#include "EntityRenderBlockmanEmpty.h"
#include "EntityRenderBuildNpc.h"
#include "EntityRenderLandNpc.h"
#include "EntityRenderFishHook.h"
#include "EntityRenderActorCannon.h"
#include "EntityRenderGunFlame.h"
#include "EntityRenderBulletin.h"
#include "EntityRenderBird.h"
#include "EntityRenderDecoration.h"
#include "EntityRenderFlyingMount.h"
#include "EntityRenderFloatingMount.h"

#include "cWorld/Blockman.h"

#include "Entity/Entity.h"
#include "Entity/EntityItem.h"
#include "Entity/EntityPlayer.h"
#include "Entity/EntityLivingBase.h"
#include "World/World.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "World/GameSettings.h"
#include "Render/RenderEntity.h"
#include "Model/ItemModelMgr.h"
#include "cItem/cItem.h"

#include "Tessolator/TessManager.h"
#include "Tessolator/SkinedRenderable.h"
#include "Tessolator/ItemRenderable.h"
#include "Tessolator/TessRenderable.h"
#include "Tessolator/FxRenderable.h"
#include "Tessolator/LineRenderable.h"
#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Scene/ModelEntity.h"
#include "Model/Mesh.h"
#include "Model/MeshManager.h"
#include "Scene/SceneManager.h"
#include "Entity/EntityVehicle.h"
#include "Setting/CarSetting.h"
#include "Entity/EntityAircraft.h"
#include "Entity/EntityActorNpc.h"
#include "Entity/EntitySessionNpc.h"
#include "Entity/EntityCreature.h"
#include "Entity/EntityBlockmanEmpty.h"
#include "cEntity/EntityActorCannonClient.h"
#include "Entity/EntityBuildNpc.h"
#include "Entity/EntityLandNpc.h"
#include "Entity/EntityBulletin.h"
#include "Entity/EntityBird.h"
#include "Entity/EntityDecoration.h"
#include "Entity/EntitySkillThrowable.h"
#include "Entity/EntityFlyingMount.h"
#include "Entity/EntityFloatingMount.h"
#include "Setting/MountSetting.h"
#include "Util/PlayerPrefs.h"
#include "Plugins/GameClientEvents.h"

namespace BLOCKMAN
{
EntityRenderManager::EntityRenderManager()
	: world(NULL)
	, itemRenderer(NULL)
	, livingPlayer(NULL)
	, pointedEntity(NULL)
	, renderPos(Vector3::ZERO)
	, playerViewY(0.f)
	, playerViewX(0.f)
	, options(NULL)
	, viewerPos(Vector3::ZERO)
{
	// load all render entities.
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_PLAYER, LordNew EntityRenderPlayer()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_ITEM, LordNew EntityRenderItem()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_ARROW, LordNew EntityRenderArrow()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_BULLET, LordNew EntityRenderBullet()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_SNOWBALL, LordNew EntityRenderThrowable(ItemClient::citemsList[ITEM_ID_SNOW_BALL])));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_POTION, LordNew EntityRenderThrowable(ItemClient::citemsList[ITEM_ID_POTION], 16384)));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_TNT_THROWABLE, LordNew EntityRenderThrowable(ItemClient::citemsList[ITEM_ID_TNT])));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_TNT_PRIMED, LordNew EntityRenderTNTPrimed()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_MERCHANT, LordNew EntityRenderMerchant()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_GRENADE, LordNew EntityRenderGrenade()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_VEHICLE, LordNew EntityRenderVehicle()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_RANK_NPC, LordNew EntityRenderRankNpc()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_AIRCRAFT, LordNew EntityRenderAircraft()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_ACTOR_NPC, LordNew EntityRenderActorNpc()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_SESSION_NPC, LordNew EntityRenderSessionNpc()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_CREATURE, LordNew EntityRenderCreature()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_CREATURE_BULLET, LordNew EntityRenderCreatureBullet()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_ITEM_SKILL, LordNew EntityRenderSkillThrowable()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_FIRE_BALL, LordNew EntityRenderThrowable(ItemClient::citemsList[ITEM_ID_FIRE_BALL])));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_BLOCKMAN_EMPTY, LordNew EntityRenderBlockmanEmpty()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_FISHHOOK_CLIENT, LordNew EntityRenderFishHook()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_ACTOR_CANNON_CLIENT, LordNew EntityRenderActorCannon()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_BUILD_NPC, LordNew EntityRenderBuildNpc()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_LAND_NPC, LordNew EntityRenderLandNpc()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_GUN_FLAME, LordNew EntityRenderGunFlame()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_BULLETIN, LordNew EntityRenderBulletin()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_BIRD, LordNew EntityRenderBird()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_DECORATION, LordNew EntityRenderDecoration()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_ROBOT, LordNew EntityRenderPlayer()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_FLYING_MOUNT, LordNew EntityRenderFlyingMount()));
	entityRenderMap.emplace(std::make_pair(ENTITY_CLASS_FLOATING_MOUNT, LordNew EntityRenderFloatingMount()));

	m_BlockTex = TextureManager::Instance()->createTexture("Blocks.png");
	m_BlockTex->load();
	
	m_ItemTex = TextureManager::Instance()->createTexture("items.png");
	m_ItemTex->load();

	m_shadowTex = TextureManager::Instance()->createTexture("shadow.png");
	m_shadowTex->load();

	m_shadowRenderable = TessManager::Instance()->createFxRenderable(m_shadowTex, FRP_ALPHA_CHANEL);
}

EntityRenderManager::~EntityRenderManager()
{
	destroyAllEntityRenderalbe();

	RendererSet allRenders;
	for (RendererMap::iterator it = entityRenderMap.begin(); it != entityRenderMap.end(); ++it)
	{
		if (allRenders.find(it->second) == allRenders.end())
			allRenders.insert(it->second);
	}
	for (RendererSet::iterator it = allRenders.begin(); it != allRenders.end(); ++it)
	{
		LordDelete(*it);
	}
	entityRenderMap.clear();

	LordSafeDeleteContainer(m_itemRenderables);
	LordSafeDeleteContainer(m_freeItemRenderables);
	LordSafeDeleteContainer(m_blockRenderables);
	LordSafeDeleteContainer(m_fullBlockRenderalbes);
	LordSafeDeleteContainer(m_freeBlockRenderables);


	TextureManager::Instance()->releaseResource(m_BlockTex);
	TextureManager::Instance()->releaseResource(m_ItemTex);
	TextureManager::Instance()->releaseResource(m_shadowTex);

	TessManager::Instance()->destroyFxRenderable(m_shadowRenderable);
}

EntityRender* EntityRenderManager::getEntityClassRenderObject(int entityClass)
{
	EntityRender* pRender = nullptr;

	auto it = entityRenderMap.find(entityClass);
	if (it == entityRenderMap.end())
	{
		int classID = Entity::getSuperClassID(entityClass);
		if (classID > 0)
		{
			pRender = getEntityClassRenderObject(classID);
			if(pRender)
				entityRenderMap.emplace(std::make_pair(classID, pRender));
		}
	}
	else
		pRender = it->second;

	return pRender;
}

EntityRender* EntityRenderManager::getEntityRenderObject(Entity* pEntity)
{
	return getEntityClassRenderObject(pEntity->getClassID());
}

EntityPlayerRenderable* EntityRenderManager::getEntityRenderable(EntityLivingBase* pLiving)
{
	EntityPlayerRenderable* result = NULL;
	auto it = m_LivingRenderables.find(pLiving);
	if (it != m_LivingRenderables.end())
		result = it->second;

	return result;
}	

LORD::ActorObject* EntityRenderManager::getEntityActor(Entity* pLiving)
{
	LORD::ActorObject* result = NULL;
	auto it = m_LivingActors.find(pLiving);
	return it != m_LivingActors.end() ? it->second : nullptr;
}

bool EntityRenderManager::createEntityRenderable(EntityLivingBase* pLiving)
{
	auto it = m_LivingRenderables.find(pLiving);
	if (it != m_LivingRenderables.end())
		return false;

	if (pLiving->isClass(ENTITY_CLASS_PLAYER))
	{
		EntityPlayer* pPlayer = (EntityPlayer*)pLiving;
		EntityPlayerRenderable* pRenderable = LordNew EntityPlayerRenderable(pPlayer, SRP_SOLID);
		m_LivingRenderables.insert(std::make_pair(pLiving, pRenderable));
	}

	return true;
}

bool EntityRenderManager::isItemMeshRender(EntityItem* pEntityItem)
{
	if (!pEntityItem)
		return false;
	ItemStackPtr pItemStack = pEntityItem->getEntityItem();
	cItem* pItem = dynamic_cast<cItem*>(pItemStack->getItem());
	if (!pItem)
		return false;
	if (pItem->getRenderType() != ItemRenderType::MESH|| pItem->getMeshName() == StringUtil::BLANK)
		return false;
	return true;
}

void EntityRenderManager::createEntityActorIfNeed(Entity* pLiving)
{
	auto it = m_LivingActors.find(pLiving);
	if (it != m_LivingActors.end())
		return;

	LORD::ActorObject* actor = nullptr;

	

	if (pLiving->isClass(ENTITY_CLASS_PLAYER))
	{
		EntityPlayer* player = static_cast<EntityPlayer*>(pLiving);
		String defaultIdle = "idle";
		if (player && player->m_defaultIdle > 0)
		{
			defaultIdle = "idle_" + StringUtil::Format("%d", player->m_defaultIdle);
		}
		bool mainActor = player && player->entityId == Blockman::Instance()->m_pPlayer->entityId;
		String actorName = "";
		if (player && player->m_sex == 2)
		{
			if (!PlayerPrefs::Instance()->HasProperty("GirlActorName"))
				actorName = "girl.actor";
			else
				actorName = PlayerPrefs::Instance()->getStringPrefs("GirlActorName");	
		}
		else 
		{
			if (!PlayerPrefs::Instance()->HasProperty("BoyActorName"))
				actorName = "boy.actor";
			else
				actorName = PlayerPrefs::Instance()->getStringPrefs("BoyActorName");
		}

		actor = ActorManager::Instance()->CreateActor(actorName, defaultIdle, false, false, mainActor);

		if (actor == NULL)
		{
			actorName = (player && player->m_sex == 2) ? "girl.actor" : "boy.actor";
			actor = ActorManager::Instance()->CreateActor(actorName, defaultIdle, false, false, mainActor);
		}

		actor->setUpper("b_spine");
		/*actor->setUpperRootParent("b_pelvis");*/
		// set user's custom skin color;
		actor->SetCustomColor(player->m_skinColor);
		actor->setBlockMan();
		actor->setUpdateManually(true);
		m_LivingActors.insert({ pLiving, actor });
	}
	else if (pLiving->isClass(ENTITY_CLASS_MERCHANT))
	{
		actor = ActorManager::Instance()->CreateActor("merchant.actor", "idle", false);
		m_LivingActors.insert({ pLiving, actor });
		actor->setBlockMan();
	}
	else if (pLiving->isClass(ENTITY_CLASS_RANK_NPC))
	{
		actor = ActorManager::Instance()->CreateActor("rank.actor", "idle", false);
		m_LivingActors.insert({ pLiving, actor });
		actor->setBlockMan();
	}
	else if (pLiving->isClass(ENTITY_CLASS_VEHICLE))
	{
		EntityVehicle* pVehicle = dynamic_cast<EntityVehicle*>(pLiving);
		if (!pVehicle)
			return;

		String model = "car01.actor";
		auto setting = pVehicle->getSetting();    
		if (setting)
			model = setting->model;

		actor = ActorManager::Instance()->CreateActor(model, "idle", false);
		if (!actor)
			return;

		m_LivingActors.insert({ pLiving, actor });
		actor->setBlockMan();
	}
	else if (pLiving->isClass(ENTITY_CLASS_AIRCRAFT))
	{
		EntityAircraft* pAircraft = dynamic_cast<EntityAircraft*>(pLiving);
		if (!pAircraft)
			return;

		actor = ActorManager::Instance()->CreateActor("plane01.actor", "walk01", false);
		if (!actor)
			return;

		m_LivingActors.insert({ pLiving, actor });
		actor->setBlockMan();
	}
	else if (pLiving->isClass(ENTITY_CLASS_ACTOR_NPC)) 
	{
		EntityActorNpc* pActorNpc = dynamic_cast<EntityActorNpc*>(pLiving);
		if (pActorNpc->getActorName().length() > 0) {
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(pActorNpc->getActorName(), "", false);
			if (!actor)
				return;

			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
	}
	else if (pLiving->isClass(ENTITY_CLASS_SESSION_NPC))
	{
		EntitySessionNpc* sessionNpc = dynamic_cast<EntitySessionNpc*>(pLiving);
		if (sessionNpc->getActorName().length() > 0) {
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(sessionNpc->getActorName(), "idle", false);
			if (!actor)
				return;

			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
	}
	else if (pLiving->isClass(ENTITY_CLASS_LAND_NPC))
	{
		EntityLandNpc* landNpc = dynamic_cast<EntityLandNpc*>(pLiving);
		if (landNpc->getActorName().length() > 0) {
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(landNpc->getActorName(), "idle", false);
			if (!actor)
				return;

			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
	}
	else if (pLiving->isClass(ENTITY_CLASS_CREATURE))
	{
		EntityCreature* pCreature = dynamic_cast<EntityCreature*>(pLiving);
		if (pCreature->getActorName().length() > 0) {
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(pCreature->getActorName(), "", false);
			if (!actor)
				return;

			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
	}
	else if (pLiving->isClass(ENTITY_CLASS_BLOCKMAN_EMPTY))
	{
		EntityBlockmanEmpty* pBlockman = dynamic_cast<EntityBlockmanEmpty*>(pLiving);
		if (!pBlockman && pBlockman->getActorName().length() > 0)
			return;
		actor = ActorManager::Instance()->CreateActor(pBlockman->getActorName(), "idle", false);
		if (!actor)
			return;

		m_LivingActors.insert({ pLiving, actor });
		actor->setBlockMan();
	}
	else if (pLiving->isClass(ENTITY_CLASS_ACTOR_CANNON_CLIENT))
	{
		EntityActorCannonClient* pActorConnon = dynamic_cast<EntityActorCannonClient*>(pLiving);
		if (pActorConnon && pActorConnon->getActorName().length() > 0)
		{
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(pActorConnon->getActorName(), "", false);
			if (!actor)
			{
				return;
			}
			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
	}
	else if (pLiving->isClass(ENTITY_CLASS_BUILD_NPC))
	{
		EntityBuildNpc* buildNpc = dynamic_cast<EntityBuildNpc*>(pLiving);
		if (buildNpc->getActorName().length() > 0) {
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(buildNpc->getActorName(), "idle", false);
			if (!actor)
				return;
			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
	}
	else if (pLiving->isClass(ENTITY_CLASS_BULLETIN))
	{
		EntityBulletin* entityBulletin = dynamic_cast<EntityBulletin*>(pLiving);
		if (entityBulletin->getActorName().length() > 0) {
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(entityBulletin->getActorName(), "idle", false);
			if (!actor)
				return;

			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
	}
	else if (pLiving->isClass(ENTITY_CLASS_BIRD))
	{
		EntityBird* entity = dynamic_cast<EntityBird*>(pLiving);
		if (entity->getActorName().length() > 0) {
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(entity->getActorName(), "idle", false);
			if (!actor)
				return;

			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
	}
	else if (pLiving->isClass(ENTITY_CLASS_ITEM_SKILL))
	{
		EntitySkillThrowable* entity = dynamic_cast<EntitySkillThrowable*>(pLiving);
		if (entity->isRenderActor() && entity->getAtlasName().length() > 0) {
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(entity->getAtlasName(), "", false);
			if (!actor)
				return;
			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
	}
	else if (pLiving->isClass(ENTITY_CLASS_DECORATION))
	{
		EntityDecoration* entity = dynamic_cast<EntityDecoration*>(pLiving);
		if (entity->getActorName().length() > 0) {
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(entity->getActorName(), "", false);
			if (!actor)
				return;
			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
	}
	else if (pLiving->isClass(ENTITY_CLASS_FLYING_MOUNT))
	{
		EntityFlyingMount* entity = dynamic_cast<EntityFlyingMount*>(pLiving);
		if (!entity)
			return;

		const auto& setting = entity->getSetting();
		actor = ActorManager::Instance()->CreateActor(setting.modelName, "idle", false);
		if (!actor)
			return;
		/*actor->SetEntityScale({ setting.scale, setting.scale, setting.scale });*/

		m_LivingActors.insert({ pLiving, actor });
		actor->setBlockMan();
		actor->setUpdateManually(true);
	}
	else if (pLiving->isClass(ENTITY_CLASS_FLOATING_MOUNT))
	{
		EntityFloatingMount* entity = dynamic_cast<EntityFloatingMount*>(pLiving);
		if (!entity)
			return;

		const auto& setting = entity->getSetting();
		actor = ActorManager::Instance()->CreateActor(setting.modelName, "idle", false);
		if (!actor)
			return;
		/*actor->SetEntityScale({ setting.scale, setting.scale, setting.scale });*/

		m_LivingActors.insert({ pLiving, actor });
		actor->setBlockMan();
		actor->setUpdateManually(true);
	}

	if (pLiving && actor)
	{
		pLiving->setSize(pLiving->getWidth(), pLiving->getLength(), pLiving->getHeight(), actor->GetOriginalScale(), true);
	}
}

bool EntityRenderManager::refreshEntityActor(Entity* pLiving, String actorName)
{
	if (pLiving->isClass(ENTITY_CLASS_PLAYER))
	{
		LORD::ActorObject* oldActor = getEntityActor(pLiving);
		
		EntityPlayer* player = static_cast<EntityPlayer*>(pLiving);
		String defaultIdle = "idle";
		if (player && player->m_defaultIdle > 0)
		{
			defaultIdle = "idle_" + StringUtil::Format("%d", player->m_defaultIdle);
		}
		bool mainActor = player && player->entityId == Blockman::Instance()->m_pPlayer->entityId;
		LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(actorName, defaultIdle, false, false, mainActor);
		if (actor)
		{
			//map<String, ActorObject::Actor_Effect>::type oldPluginEffects{};
			if (oldActor)
			{
				//oldPluginEffects = oldActor->getPluginEffects();
				destroyEntityActor(pLiving);
			}
			actor->setUpper("b_spine");
			/*actor->setUpperRootParent("b_pelvis");*/
			actor->SetCustomColor(player->m_skinColor);
			m_LivingActors.insert({ pLiving, actor });
			player->m_isPeopleActor = false;
			player->m_outLooksChanged = true;
			player->m_isNeedChangePostion = true;
			player->m_holdItemChanged = true;
			actor->setBlockMan();
			if (player->m_moving && player->onGround)
			{
				actor->PlaySkill("run");
			}
		}
		return true;
	}
	else if (pLiving->isClass(ENTITY_CLASS_ACTOR_NPC))
	{
		LORD::ActorObject* oldActor = getEntityActor(pLiving);
		EntityActorNpc* pActorNpc = dynamic_cast<EntityActorNpc*>(pLiving);
		LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(pActorNpc->getActorName(), pActorNpc->getSkillName(), false);
		if (!actor)
			return false;
		if (oldActor)
		{
			destroyEntityActor(pLiving);
		}
		m_LivingActors.insert({ pLiving, actor });
		actor->setBlockMan();
		return true;
	}
	else if (pLiving->isClass(ENTITY_CLASS_SESSION_NPC))
	{
		LORD::ActorObject* oldActor = getEntityActor(pLiving);
		EntitySessionNpc* pActorNpc = dynamic_cast<EntitySessionNpc*>(pLiving);
		String actorFileName = oldActor ? oldActor->GetActorFileName() : "";
		if (actorFileName == pActorNpc->getActorName() && oldActor)
		{
			oldActor->setBlockMan();
		}
		else
		{
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(pActorNpc->getActorName(), "idle", false);
			if (!actor)
				return false;
			if (oldActor)
			{
				destroyEntityActor(pLiving);
			}
			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
		return true;
	}
	else if (pLiving->isClass(ENTITY_CLASS_LAND_NPC))
	{
		LORD::ActorObject* oldActor = getEntityActor(pLiving);
		EntityLandNpc* pActorNpc = dynamic_cast<EntityLandNpc*>(pLiving);
		String actorFileName = oldActor ? oldActor->GetActorFileName() : "";
		if (actorFileName == pActorNpc->getActorName() && oldActor)
		{
			oldActor->setBlockMan();
		}
		else
		{
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(pActorNpc->getActorName(), "idle", false);
			if (!actor)
				return false;
			if (oldActor)
			{
				destroyEntityActor(pLiving);
			}
			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
		return true;
	}
	else if (pLiving->isClass(ENTITY_CLASS_CREATURE))
	{
		LORD::ActorObject* oldActor = getEntityActor(pLiving);
		if (oldActor)
		{
			destroyEntityActor(pLiving);
		}
		EntityCreature* pActorCreature = dynamic_cast<EntityCreature*>(pLiving);
		LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(pActorCreature->getActorName(), "", 0, false, true, true);
		if (!actor)
			return false;

		m_LivingActors.insert({ pLiving, actor });
		actor->setBlockMan();
		return true;
	}
	else if (pLiving->isClass(ENTITY_CLASS_BLOCKMAN_EMPTY))
	{
		LORD::ActorObject* oldActor = getEntityActor(pLiving);
		auto blockman = dynamic_cast<EntityBlockmanEmpty*>(pLiving);
		LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(blockman->getActorName(), "idle", false);
		if (!actor)
			return false;
		if (oldActor)
		{
			destroyEntityActor(pLiving);
		}
		m_LivingActors.insert({ pLiving, actor });
		blockman->m_isActorChange = true;
		actor->setBlockMan();
		return true;
	}
	else if (pLiving->isClass(ENTITY_CLASS_ACTOR_CANNON_CLIENT))
	{
		LORD::ActorObject* oldActor = getEntityActor(pLiving);
		EntityActorCannonClient* pActorCannon = dynamic_cast<EntityActorCannonClient*>(pLiving);
		LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(pActorCannon->getActorName(), pActorCannon->getSkillName(), false, true);
		if (!actor)
			return false;
		if (oldActor)
		{
			destroyEntityActor(pLiving);
		}
		m_LivingActors.insert({ pLiving, actor });
		actor->setBlockMan();
		return true;
	}
	else if (pLiving->isClass(ENTITY_CLASS_BUILD_NPC))
	{
		LORD::ActorObject* oldActor = getEntityActor(pLiving);
		EntityBuildNpc* pActorNpc = dynamic_cast<EntityBuildNpc*>(pLiving);
		String actorFileName = oldActor ? oldActor->GetActorFileName() : "";
		if (actorFileName == pActorNpc->getActorName() && oldActor)
		{
			oldActor->setBlockMan();
		}
		else 
		{
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(pActorNpc->getActorName(), "idle", false);
			if (!actor)
				return false;
			if (oldActor)
			{
				destroyEntityActor(pLiving);
			}
			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
		return true;
	}
	else if (pLiving->isClass(ENTITY_CLASS_BULLETIN))
	{
		LORD::ActorObject* oldActor = getEntityActor(pLiving);
		EntityBulletin* pActorNpc = dynamic_cast<EntityBulletin*>(pLiving);
		String actorFileName = oldActor ? oldActor->GetActorFileName() : "";
		if (actorFileName == pActorNpc->getActorName() && oldActor)
		{
			oldActor->setBlockMan();
		}
		else
		{
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(pActorNpc->getActorName(), "idle", false);
			if (!actor)
				return false;
			if (oldActor)
			{
				destroyEntityActor(pLiving);
			}
			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
		return true;
	}
	else if (pLiving->isClass(ENTITY_CLASS_BIRD))
	{
		LORD::ActorObject* oldActor = getEntityActor(pLiving);
		EntityBird* entity = dynamic_cast<EntityBird*>(pLiving);
		String actorFileName = oldActor ? oldActor->GetActorFileName() : "";
		if (actorFileName == entity->getActorName() && oldActor)
		{
			oldActor->setBlockMan();
		}
		else
		{
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(entity->getActorName(), "idle", false);
			if (!actor)
				return false;
			if (oldActor)
			{
				destroyEntityActor(pLiving);
			}
			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
		}
		return true;
	}
	else if (pLiving->isClass(ENTITY_CLASS_DECORATION))
	{
		LORD::ActorObject* oldActor = getEntityActor(pLiving);
		EntityDecoration* entity = dynamic_cast<EntityDecoration*>(pLiving);
		String actorFileName = oldActor ? oldActor->GetActorFileName() : "";
		if (actorFileName == entity->getActorName() && oldActor)
		{
			oldActor->setBlockMan();
		}
		else
		{
			LORD::ActorObject* actor = ActorManager::Instance()->CreateActor(entity->getActorName(), "idle", false);
			if (!actor)
				return false;
			if (oldActor)
			{
				destroyEntityActor(pLiving);
			}
			m_LivingActors.insert({ pLiving, actor });
			actor->setBlockMan();
			actor->setUpdateManually(true);
		}
		return true;
	}
	return false;
}

bool EntityRenderManager::restorePlayerActor(Entity* pLiving)
{
	if (pLiving->isClass(ENTITY_CLASS_PLAYER))
	{
		LORD::ActorObject* oldActor = getEntityActor(pLiving);
		if (oldActor)
		{
			destroyEntityActor(pLiving);
		}
		EntityPlayer* player = static_cast<EntityPlayer*>(pLiving);
		String defaultIdle = "idle";
		if (player && player->m_defaultIdle > 0)
		{
			defaultIdle = "idle_" + StringUtil::Format("%d", player->m_defaultIdle);
		}
		bool mainActor = player && player->entityId == Blockman::Instance()->m_pPlayer->entityId;
		String actorName = "";
		if (player && player->m_sex == 2)
		{
			if (!PlayerPrefs::Instance()->HasProperty("GirlActorName"))
				actorName = "girl.actor";
			else
				actorName = PlayerPrefs::Instance()->getStringPrefs("GirlActorName");
		}
		else
		{
			if (!PlayerPrefs::Instance()->HasProperty("BoyActorName"))
				actorName = "boy.actor";
			else
				actorName = PlayerPrefs::Instance()->getStringPrefs("BoyActorName");
		}
		LORD::ActorObject* actor = nullptr;
		actor = ActorManager::Instance()->CreateActor(actorName, defaultIdle, false, false, mainActor);
		if (actor == NULL)
		{
			actorName = (player && player->m_sex == 2) ? "girl.actor" : "boy.actor";
			actor = ActorManager::Instance()->CreateActor(actorName, defaultIdle, false, false, mainActor);
		}
		actor->setUpper("b_spine");
		/*actor->setUpperRootParent("b_pelvis");*/
		// set user's custom skin color;
		actor->SetCustomColor(player->m_skinColor);
		actor->setUpdateManually(true);
		m_LivingActors.insert({ pLiving, actor });
		player->m_isPeopleActor = true;
		player->m_outLooksChanged = true;
		return true;
	}
	return false;
}

bool EntityRenderManager::createMeshItem(EntityItem* pEntityItem)
{
	auto it = m_meshItems.find(pEntityItem);
	if (it != m_meshItems.end())
		return false;

	cItem* citem = dynamic_cast<cItem*>(pEntityItem->getEntityItem()->getItem());
	if (citem->getRenderType() != ItemRenderType::MESH)
		return false;
	if (citem->getMeshName() == StringUtil::BLANK)
		return false;
	LORD::Mesh* pMesh = MeshManager::Instance()->createMesh(citem->getMeshName());
	pMesh->load();
	LORD::ModelEntity* entity = LordNew LORD::ModelEntity;
	LORD::SceneNode* node = LORD::SceneManager::Instance()->getRootNode()->createChild();
	entity->setMesh(pMesh);
	entity->attachTo(node);
	entity->setMaterial(ActorManager::Instance()->GetStaticMaterial());
	entity->createRenderable(false);
	entity->prepareTexture(true);
	m_meshItems.insert({ pEntityItem, entity });
	return true;
}

bool EntityRenderManager::destroyMeshItem(EntityItem* pEntityItem)
{
	auto it = m_meshItems.find(pEntityItem);
	if (it == m_meshItems.end())
		return false;

	LORD::ModelEntity* entity = it->second;
	LORD::SceneNode* node = entity->getSceneNode();
	LORD::Mesh* mesh = entity->getMesh();
	LORD::SceneManager::Instance()->getRootNode()->destroyChild(node);
	LORD::MeshManager::Instance()->releaseResource(mesh);
	LordDelete(it->second);
	m_meshItems.erase(it);
	return true;
}

LORD::ModelEntity* EntityRenderManager::getMeshItem(EntityItem* pEntityItem)
{
	auto it = m_meshItems.find(pEntityItem);
	if (it != m_meshItems.end())
		return it->second;
	return NULL;
}
bool EntityRenderManager::destroyEntityRenderable(EntityLivingBase* pLiving)
{
	auto it = m_LivingRenderables.find(pLiving);
	if (it == m_LivingRenderables.end())
		return false;

	LordDelete(it->second);
	m_LivingRenderables.erase(it);
	return true;
}

void EntityRenderManager::destroyAllEntityRenderalbe()
{
	for (auto& it : m_LivingRenderables)
		LordDelete(it.second);
	m_LivingRenderables.clear();
}

bool EntityRenderManager::destroyEntityActor(Entity* pLiving)
{
	auto it = m_LivingActors.find(pLiving);
	if (it == m_LivingActors.end())
		return false;

	ActorManager::Instance()->DestroyActor(it->second, false);
	m_LivingActors.erase(it);
	return true;
}

void EntityRenderManager::destroyAllEntityActor()
{
	for (auto it : m_LivingActors)
	{
		ActorObject* actor = it.second;
		if(actor)
			ActorManager::Instance()->DestroyActor(actor, false);
	}
	m_LivingActors.clear();
}

void EntityRenderManager::cacheActiveRenderInfo(World* pWorld, EntityLivingBase* pLiving1, EntityLivingBase* pLiving2, GameSettings* setting, float ticks)
{
	world = pWorld;
	rendererEntity = Blockman::Instance()->m_entityRender;
	options = setting;
	livingPlayer = pLiving1;
	pointedEntity = pLiving2;
	// fontRenderer = par3FontRenderer;

	if (pLiving1->isPlayerSleeping())
	{
		int ix = int(Math::Floor(pLiving1->getPosition().x));
		int iy = int(Math::Floor(pLiving1->getPosition().y));
		int iz = int(Math::Floor(pLiving1->getPosition().z));
		int blockID = pWorld->getBlockId(BlockPos(ix, iy, iz));

		if (blockID == BLOCK_ID_BED)
		{
			int meta = pWorld->getBlockMeta(BlockPos(ix, iy, iz));
			int dir = meta & 3;
			playerViewY = (float)(dir * 90 + 180);
			playerViewX = 0.0F;
		}
	}
	else
	{
		playerViewY = pLiving1->prevRotationYaw + (pLiving1->rotationYaw - pLiving1->prevRotationYaw) * ticks;
		playerViewX = pLiving1->prevRotationPitch + (pLiving1->rotationPitch - pLiving1->prevRotationPitch) * ticks;
	}

	if (setting->getPersonView() == GameSettings::SPVT_SECOND)
	{
		playerViewY += 180.0F;
	}

	viewerPos = pLiving1->getLastTickPosition(ticks);
}

void EntityRenderManager::renderEntity(Entity* pEntity, float ticks)
{
	if (pEntity->ticksExisted == 0)
	{
		pEntity->lastTickPos = pEntity->position;
	}

	Vector3 pos = pEntity->lastTickPos + (pEntity->position - pEntity->lastTickPos) * ticks;
	float yaw = pEntity->prevRotationYaw + (pEntity->rotationYaw - pEntity->prevRotationYaw) * ticks;
	renderEntityWithPosYaw(pEntity, pos, yaw, ticks);
}

void EntityRenderManager::renderEntityWithPosYaw(Entity* pEntity, const Vector3& pos, float yaw, float ticks)
{
	EntityRender* pRender = NULL;

	if (pEntity)
	{
		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(pEntity);
		if (actor)
		{
			ActorObject* currParentActor = actor->GetParentActor();
			if (!currParentActor)
			{
				ActorObject* parentActor = EntityRenderManager::Instance()->getEntityActor(pEntity->getParentEntity());
				if (parentActor)
				{
					Quaternion quat1(Vector3(1.0f, 0.0f, 0.0f), -Math::PI / 2);
					Quaternion quat2(Vector3(0.0f, 0.0f, 1.0f), Math::PI);
					/*actor->setAttachParam(quat1 * quat2, Vector3::ZERO, -1.0f);*/
					parentActor->AttachChildActor(pEntity->getParentActorConnectiveBone(), actor);
				}
			}
			else
			{
				ActorObject* parentActor = EntityRenderManager::Instance()->getEntityActor(pEntity->getParentEntity());
				if (currParentActor != parentActor)
				{
					currParentActor->DetachChildActor(actor);
					if (parentActor)
					{
						Quaternion quat1(Vector3(1.0f, 0.0f, 0.0f), -Math::PI / 2);
						Quaternion quat2(Vector3(0.0f, 0.0f, 1.0f), Math::PI);
						/*actor->setAttachParam(quat1 * quat2, Vector3::ZERO, -1.0f);*/
						parentActor->AttachChildActor(pEntity->getParentActorConnectiveBone(), actor);
					}
				}
				/*actor->SetEntityScale(actor->GetEntityScale());*/
			}
		}
	}

	pRender = getEntityRenderObject(pEntity);

	if (pRender)
	{
		pRender->doRender(pEntity, pos, yaw, ticks);	
		pRender->doRenderShadowAndFire(pEntity, pos, yaw, ticks);
		pRender->doRenderSelectedNode(pEntity, pos, yaw, ticks);
		pRender->doRenderEntityFeature(pEntity, ticks);
	}
}

void EntityRenderManager::renderBoudingBox(Entity* pEntity, const Vector3& pos, float yaw, float ticks)
{
}

/** World sets this RenderManager's worldObj to the world provided */
void EntityRenderManager::set(World* pWorld)
{
	world = pWorld;
}

float EntityRenderManager::getDistanceToCamera(const Vector3& pos)
{
	return (pos - viewerPos).lenSqr();
}

/** Returns the font renderer */
// FontRenderer getFontRenderer() { return fontRenderer; }

void EntityRenderManager::updateIcons(TextureAtlasRegister* atlas)
{
	for (RendererMap::iterator it = entityRenderMap.begin(); it != entityRenderMap.end(); ++it)
	{
		EntityRender* pRender = it->second;
		if (!pRender)
			pRender->updateIcons(atlas);
	}
}

void EntityRenderManager::beginRender()
{
	// put all the inuse renderables to free queue.
	m_freeItemRenderables.insert(m_freeItemRenderables.end(), m_itemRenderables.begin(), m_itemRenderables.end());
	m_itemRenderables.clear();

	m_freeBlockRenderables.insert(m_freeBlockRenderables.end(), m_blockRenderables.begin(), m_blockRenderables.end());
	m_freeBlockRenderables.insert(m_freeBlockRenderables.end(), m_fullBlockRenderalbes.begin(), m_fullBlockRenderalbes.end());
	m_blockRenderables.clear();
	m_fullBlockRenderalbes.clear();

	m_shadowRenderable->beginPolygon();
}

void EntityRenderManager::endRender()
{
	// put all the ItenRenderable to tessmanager.
	ItemRenderable* pItemRenderable = NULL;
	for (ItemTessRenderableArr::iterator it = m_itemRenderables.begin(); it != m_itemRenderables.end(); ++it)
	{
		pItemRenderable = *it;
		if (pItemRenderable->getRenderGroup() == IRP_ENCHANT || pItemRenderable->getRenderGroup() == IRP_ENCHANT_NODEPTH)
			TessManager::Instance()->addEnchantRenderable(pItemRenderable);
		else
			TessManager::Instance()->addItemRenderalbe(pItemRenderable);
	}

	PackMassRenderable* pSkinedRenderable = NULL;
	for (ItemBlockRenderableArr::iterator it = m_blockRenderables.begin(); it != m_blockRenderables.end(); ++it)
	{
		pSkinedRenderable = *it;
		if (pSkinedRenderable)
		{
			pSkinedRenderable->endPolygon();
			pSkinedRenderable->updateRenderQueue();
		}
	}
	for (ItemBlockRenderableArr::iterator it = m_fullBlockRenderalbes.begin(); it != m_fullBlockRenderalbes.end(); ++it)
	{
		pSkinedRenderable = *it;
		if (pSkinedRenderable)
		{
			pSkinedRenderable->endPolygon();
			pSkinedRenderable->updateRenderQueue();
		}
	}

	m_shadowRenderable->endPolygon();
	if (m_shadowRenderable->getQuadNum() > 0)
		TessManager::Instance()->addFxRenderable(m_shadowRenderable);
}

ItemRenderable* EntityRenderManager::getItemRenderable()
{
	ItemRenderable* pItemRenderable = NULL;
	if (!m_freeItemRenderables.empty())
	{
		pItemRenderable = m_freeItemRenderables.front();
		m_freeItemRenderables.pop_front();
	}
	else
	{
		pItemRenderable = LordNew ItemRenderable();
	}

	m_itemRenderables.push_back(pItemRenderable);
	return pItemRenderable;
}

PackMassRenderable* EntityRenderManager::getPackRenderable(Texture* tex, SKINED_RENDER_PASS rq, int quadNum)
{
	PackMassRenderable* pPackRenderalbe = NULL;

	// fetch from the inuseing queue.
	for (ItemBlockRenderableArr::iterator it = m_blockRenderables.begin(); it != m_blockRenderables.end(); )
	{
		pPackRenderalbe = *it;
		int freeQuadNum = pPackRenderalbe->getFreeQuadNum();
		int freeBoneNum = pPackRenderalbe->getFreeBoneCount();
		if (freeQuadNum < quadNum || freeBoneNum <= 0)
		{
			m_fullBlockRenderalbes.push_back(pPackRenderalbe);
			ItemBlockRenderableArr::iterator jt = it++;
			m_blockRenderables.erase(jt);
			continue;
		}
		
		if (pPackRenderalbe->getRenderGroup() == rq &&
			pPackRenderalbe->getTexture() == tex)
		{
			return pPackRenderalbe;
		}
		++it;
	}

	if (!m_freeBlockRenderables.empty())
	{
		pPackRenderalbe = m_freeBlockRenderables.front();
		m_freeBlockRenderables.pop_front();

		pPackRenderalbe->setMaterialParam(tex, rq);
	}
	else
	{
		pPackRenderalbe = LordNew PackMassRenderable(tex, rq);
	}

	m_blockRenderables.push_back(pPackRenderalbe);
	pPackRenderalbe->beginPolygon();

	return pPackRenderalbe;
}

}
