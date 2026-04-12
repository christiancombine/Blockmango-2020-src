#include "WorldClient.h"
#include "Blockman.h"
#include "PlayerInteractionManager.h"
#include "UnitTest.h"
#include "Chunk/ChunkService.h"

#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "cItem/cItem.h"
#include "cEntity/PlayerControlller.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "EntityRenders/EntityRenderManager.h"
#include "EntityRenders/EntityRenderPlayer.h"
#include "EntityRenders/EntityRenderable.h"
#include "cEffects/EffectRenderer.h"
#include "cTileEntity/TileEntityRenderMgr.h"
#include "Model/ItemModelMgr.h"
#include "Render/TextureAtlas.h"
#include "Render/RenderGlobal.h"
#include "Render/Colorizer.h"
#include "Render/RenderEntity.h"
#include "Render/ItemRender.h"
#include "Render/VisualGraph.h"
#include "Render/HeadTextRender.h"
#include "Network/ClientNetwork.h"
#include "Network/ClientPacketSender.h"
#include "EntityRenders/EntityRenderBiped.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Render/RenderStage.h"
#include "Util/CPUTimer.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Block/BlockManager.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Item/Potion.h"
#include "Item/PotionManager.h"
#include "Inventory/Container.h"
#include "Inventory/CraftingManager.h"
#include "Inventory/InventoryPlayer.h"
#include "TileEntity/TileEntity.h"
#include "TileEntity/TileEntityInventory.h"
#include "Entity/Attributes.h"
#include "Entity/DamageSource.h"
#include "Entity/Enchantment.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityLivingBase.h"
#include "Entity/EntityCreature.h"
#include "World/World.h"
#include "World/GameSettings.h"
#include "Chunk/Chunk.h"
#include "World/Section.h"
#include "World/WorldSettings.h"
#include "World/RayTracyResult.h"
#include "Util/Facing.h"
#include "Chunk/ChunkService.h"

#include "GUI/GuiDef.h"
#include "GUI/RootGuiLayout.h"
#include "Util/ClientEvents.h"
#include "Util/PlayerPrefs.h"

#include "UI/GUIWindowManager.h"
#include "UI/GUISystem.h"
#include "UI/GUISubscriberSlot.h"
#include "Render/Renderer.h"
#include "Object/Root.h"
#include "Util/QueryObject.h"
#include "cWorld/BlockChangeRecorderClient.h"
#include "ShellInterface.h"
#include "game.h"
#include "Audio/SoundSystem.h"
#include "Util/ClientCmdMgr.h"
#include "cWorld/SignTextsChangeRecorder.h"
#include "Actor/ActorObject.h"
#include "GUI/GuiItemStack.h"
#include "Entity/EntityMerchant.h"
#include "Entity/EntityRankNpc.h"
#include "Entity/EntityActorNpc.h"
#include "cWorld/RankManager.h"
#include "Entity/EntitySessionNpc.h"
#include "Render/VisualTessThread.h"
#include "Actor/ActorManager.h"
#include "cEntity/EntityActorCannonClient.h"
#include "Entity/EntityBuildNpc.h"
#include "Entity/EntityBlockman.h"
#include "Entity/EntityLandNpc.h"
#include "Entity/EntityDecoration.h"
#include "Setting/GatherToolSetting.h"
#include "Setting/LogicSetting.h"
#include "Util/UICommon.h"

#include "LuaRegister/Template/LuaEngine.h"
#include "Script/Setting/ScriptSetting.h"
#include "Plugins/PluginManager.h"
#include "Plugins/GameClientEvents.h"
#include "Entity/TeleportPositionConverter.h"

namespace BLOCKMAN
{

const float Blockman::s_tickFPS = 1.f / 20.f;
void Blockman::initialize()
{
	LordNew BlockChangeRecorderClient;
	LordNew SignTextsChangeRecorder;

   	cBlockManager::cInitialize();
	TileEntity::initialize();
	SharedMonsterAttributes::initialize();
	DamageSource::initialize();
	Enchantment::initialize();
	EnchantmentHelper::initialize();
	EntityLivingBase::initialize();
	ToolMaterial::initialize();
	ArmorMaterial::initialize();
	Potion::initialize();
	PotionManager::initialize();
	ItemClient::cInitialize();
	CraftingManager* pCrafting = LordNew CraftingManager();
	PotionHelper::initialize();
	TileEntityBeacon::initialize();
	ItemRenderer::initialize();
	LordNew HeadTextRenderManager();
	registerCustomUiWidget();
	// todo. these all json register need to build a config file.
	TextureAtlasRegister* atlasRegister = LordNew TextureAtlasRegister;
	atlasRegister->addMemTextureAtlas("Blocks.json", "Blocks.json");
	atlasRegister->addTextureAtlas("water_still.json");
	atlasRegister->addTextureAtlas("water_flow.json");
	atlasRegister->addTextureAtlas("portal.json");
	atlasRegister->addTextureAtlas("lava_still.json");
	atlasRegister->addTextureAtlas("lava_flow.json");
	atlasRegister->addTextureAtlas("fire_layer_1.json");
	atlasRegister->addTextureAtlas("fire_layer_0.json");
	atlasRegister->addTextureAtlas("items.json");
	atlasRegister->addTextureAtlas("clock.json");
	atlasRegister->addTextureAtlas("compass.json");
	atlasRegister->addTextureAtlas("armor.json");
	atlasRegister->addTextureAtlas("blood_strip.json");
	atlasRegister->addTextureAtlas("float_number.json");
	atlasRegister->addTextureAtlas("game_items.json");

	GuiItemStack::initialize();
	cBlockManager::registSprite(atlasRegister);
	ItemClient::registSprite(atlasRegister);
	Colorizer* colorizer = LordNew Colorizer();
	colorizer->initColorBuffer(COLORIZER_TYPE_GRASS, "grass.png");
	colorizer->initColorBuffer(COLORIZER_TYPE_FOLIAGE, "foliage.png");
	VisualGraph::initialize();
	Facing::initialize();
	SetVisibility::initialize();
	ItemModelManager* itemModelMgr = LordNew ItemModelManager();
	// itemModelMgr->setAtlasFor32PixBlock("Blocks.json");

	MemTextureAtlas* blockAtlas = dynamic_cast<MemTextureAtlas*>(atlasRegister->getTextureAtlas("Blocks.json"));
	itemModelMgr->initBlockImage(blockAtlas);
	itemModelMgr->setImage(static_cast<int>(ItemRenderType::TEXTURE), "items.png");
	itemModelMgr->setImage(static_cast<int>(ItemRenderType::COMPASS), "compass.png");
	itemModelMgr->setImage(static_cast<int>(ItemRenderType::CLOCK), "clock.png");
	itemModelMgr->setImage(static_cast<int>(ItemRenderType::GAME_TEXTURE), "game_items.png");
	ClientCmdMgr::init();

}

void Blockman::unInitialize()
{
	LordDelete(HeadTextRenderManager::Instance());
	LordDelete(ItemModelManager::Instance());

	ItemRenderer::unInitialize();
	Potion::unInitialize();
	ArmorMaterial::unInitialize();
	ToolMaterial::unInitialize();
	EntityLivingBase::unInitialize();
	DamageSource::unInitialize();
	SharedMonsterAttributes::unInitialize();
	EnchantmentHelper::unInitalize();
	Enchantment::unInitialize();
	EntityRenderBiped::unInitialize();
	Colorizer* colorizer = Colorizer::Instance();
	LordDelete(colorizer);
	TextureAtlasRegister* atlasRegister = TextureAtlasRegister::Instance();
	LordDelete(atlasRegister);
    GuiItemStack::unInitialize();
	VisualGraph::unInitialize();
	Facing::unInitialize();
	SetVisibility::unInitialize();
	PotionManager::unInitialize();
//    Item::unInitialize();
    ItemClient::cUnInitialize();
	LordDelete(CraftingManager::Instance());
	cBlockManager::cUnInitialize();
    EntityRenderPlayer::unInitialize();
    
	LordDelete(SignTextsChangeRecorder::Instance());
	LordDelete(BlockChangeRecorderClient::Instance());
}

Blockman::Blockman()
	: m_pWorld(NULL)
	, m_worldSettings(NULL)
	, m_gameSettings(NULL)
	, m_globalRender(NULL)
	, m_entityRender(NULL)
	//, m_mainControl(NULL)
	, renderViewEntity(NULL)
	, pointedEntityLiving(NULL)
	, m_clickCounter(0)
	, m_playerControl(NULL)
	, m_pPlayer(NULL)
	, joinPlayerCounter(0)
	, debugCrashKeyPressTime(0)
	, m_lastUseItemId(-1)
	, m_teleportIdle(0)
	, lastPos(Vector3::ZERO)
	, exitPressed(false)
	, longtouchTick(4)
{
	m_worldName = "newWorld1";
	m_logicSystemTime = getSystemTime();
}

Blockman::~Blockman()
{
	// delete all the backgroud thead chunkProvide request.	
	if (m_visualTessThread)
	{
		if (m_visualTessThread->IsRunning())
		{
			m_visualTessThread->Stop();
			delete m_visualTessThread;
			m_visualTessThread = nullptr;
		}
	}

	LordSafeDelete(m_playerControl);
	LordSafeDelete(m_interactionMgr);
	Root::Instance()->enableStreamThread(false);

	EntityRenderManager::Instance()->destroyAllEntityActor();
	LORD::ActorManager::Instance()->DestroyDelayActors(true);

	if (m_pPlayer)
		LordSafeDelete(m_pPlayer->movementInput);
	// LordSafeDelete(m_pPlayer);
	LordDelete(TileEntityRenderMgr::Instance());
	LordDelete(EffectRenderer::Instance());
	LordDelete(EntityRenderManager::Instance());
	LordSafeDelete(m_entityRender);
	//LordDelete(ChunkRebuilder::Instance());
	LordSafeDelete(m_globalRender);
	LordSafeDelete(m_pWorld);
	LordSafeDelete(m_gameSettings);
	LordSafeDelete(m_worldSettings);
	LordSafeDelete(m_rootGuiLayout);
}

i64 test_rand[] = {
	3315535095586843436LL,
	7811472220827660346LL,
	27977402478636135LL,
	-2961405825330349164LL,
	-9039153669293870600LL,
	6727893985866532711LL,
	-1777905325754646240LL,
	-3969516832809676251LL,
	-3378829271918789601LL,
	-2720273531442913371LL
};

int testIndex = 4;


void Blockman::init()
{
	const i64 seed = test_rand[testIndex];
	m_worldSettings = LordNew WorldSettings(seed, GAME_TYPE_SURVIVAL, false, false, TERRAIN_TYPE_DEFAULT);
	m_gameSettings = LordNew GameSettings();
	m_pWorld = LordNew WorldClient(m_worldName, *m_worldSettings, m_gameSettings->getLoadChunksRange());
	// virtual function World::createChunkService must call obviously.
	m_pWorld->createChunkService(m_gameSettings->getLoadChunksRange());

	m_globalRender = LordNew RenderGlobal(this);
	// m_globalRender->setWorldAndLoadRenderers(m_pWorld);
	m_globalRender->registerDestroyBlockIcons(TextureAtlasRegister::Instance());
	m_entityRender = LordNew RenderEntity(this);
	// LordNew ChunkRebuilder;

	

	//create Root window 
	m_rootGuiLayout = LordNew RootGuiLayout;
	m_rootGuiLayout->show();
	/******/

	m_playerControl = LordNew PlayerController(this);
	EntityRenderManager* entityRM = LordNew EntityRenderManager();
	EffectRenderer* effectRM = LordNew EffectRenderer(m_pWorld);
	TileEntityRenderMgr* tentityM = LordNew TileEntityRenderMgr();
	EntityRenderPlayer::setDefaultTex("boy.png");
	m_interactionMgr = LordNew PlayerInteractionManager(m_pWorld);

	m_tickTime = 0.f;
	m_render_dt = 0.f;

	testMathLib();

	// begin thread
	m_visualTessThread = new VisualTessThread();
	m_visualTessThread->Start();

	m_subscriptionGuard.add(AllowUseItemStatusEvent::subscribe(std::bind(&Blockman::setAllowUseItem, this, std::placeholders::_1)));
}

void Blockman::tick(float dt)
{
	PROFILE_CPU("Blockman::tick");
	if (dt > 0.4f)
		dt = 0.4f;

	BLOCKMAN::PROMISE::runCallbacks();

	if (m_pPlayer)
	{
		Vector3 pos = m_pPlayer->position;
		Vector2 chunkDiff(pos.x - lastPos.x, pos.z - lastPos.z);
		if (chunkDiff.len() >= 16.f)
		{
			if (m_firstLogin)
				m_firstLogin = false;
			else
				m_teleportIdle = 50;
			m_globalRender->teleport();
			LordLogInfo("Blockman teleport from (%d,%d,%d) to (%d,%d,%d)"
				, int(lastPos.x), int(lastPos.y), int(lastPos.z)
				, int(pos.x), int(pos.y), int(pos.z));
			// camera need set pos!
			m_pPlayer->prevPos = m_pPlayer->position;
		}
		lastPos = pos;

		m_pWorld->m_pChunkService->setCenter(int(Math::Floor(pos.x)) >> 4, int(Math::Floor(pos.z)) >> 4);
		
		if (m_teleportIdle <= 0)
		{
			WorldClient* pclientWorld = (WorldClient*)m_pWorld;
			pclientWorld->asyncLoadChunks(pos, m_gameSettings->getRenderRange() / 16);
		}
		else
		{
			--m_teleportIdle;
		}
	}

	// m_pWorld->tick();
	// m_entityRender->updateRenderer();

	m_tickTime += dt;
	while (m_tickTime >= s_tickFPS)
	{
		runTick();
		m_tickTime -= s_tickFPS;
	}

	// make sure debug mode has a stable frame rate.
	m_render_dt = m_tickTime / s_tickFPS;
	m_entityRender->updateCameraAndRender(dt, m_render_dt);
}

String Blockman::getDebugString()
{
	i64 time = m_pWorld->getWorldTime();
	int hours = (int)(time / 1000) % 24;
	hours += 8;
	if (hours >= 24)
		hours -= 24;
	int mins = int((time % 1000) / 1000.f * 60.f);
	return StringUtil::Format(
		"Tasks:%d render:%u update:%u rasterCull:%d fxs:%d Time:%d:%d current time:%u",
		m_pWorld->m_pChunkService->getAsyncCount(),
		// ChunkRebuilder::Instance()->getTaskCount(),
		// m_globalRender->getUpdateCount(),
		m_globalRender->getRenderersBeingRendered(),
		m_globalRender->getRenderersUpdateCount(),
		m_globalRender->getRenderersRasterCullCnt(),
		EffectRenderer::Instance()->getFxCount(),
		hours, mins, LORD::Root::Instance()->getCurrentTime()/1000);
}

String Blockman::getPlayerString()
{
	Vector3 currentPos = m_pPlayer->getPosition(m_render_dt);
	float pitch = m_pPlayer->rotationPitch;
	float yaw = m_pPlayer->rotationYaw;
	float health = m_pPlayer->getHealth();
	return StringUtil::Format("Pos x:%.2f y:%.2f z:%.2f pitch:%.1f yaw:%.1f health:%.1f",
		currentPos.x, currentPos.y - 1.62, currentPos.z, pitch, yaw, health);
}

void Blockman::caculateCpuTime()
{
	/*
			begin         activeStage1             activeStage2                                                  finish
			  |                 |                      |                                                           |
	MainThread|-----preUpdate---|-----preGFX-----------|-----------------------Lord--------------------------------|
			  |                 |                       \                                                         /|
			  |                 |                        \                                                       / |
			  |                 |                         \                                                     /  |
			  |                 |                          \                                                   /   |
			  |                 |                           \                                                 /    |
			  |                 |                            \                                               /     |
			  |                 |                             \                                             /      |
			  |                 |                              \                                           /       |
	MeshThread|                 |---FindVisible--|---Lighting---|--------TileMeshGen----------------------|        |
			  |                 |                |              |                                         |        |
											  lighting        meshBegin                                meshEnd
	*/

	static int tickcnt = 0;
	static int lasttickcnt = 1;

	static ui64 total = 0;
	static ui64 preupdate = 0;
	static ui64 preGfx = 0;
	static ui64 lord = 0;
	static ui64 waitactive = 0;
	static ui64 visibi = 0;
	static ui64 findvis = 0;
	static ui64 lighting = 0;
	static ui64 meshgen = 0;
	static ui64 waiting1 = 0;
	static ui64 waiting2 = 0;
	tickcnt++;

	if (tickcnt > 1)
	{
		total      += (m_asyncEnd - m_mainBegin);
		preupdate  += (m_exchangeBegin - m_mainBegin);    // main-thread
		preGfx     += (m_renderBegin - m_exchangeBegin);  // main-thread
		lord       += (m_asyncBegin - m_renderBegin);     // main-thread
		waitactive += (m_visibiBegin - m_waitActive);
		visibi     += (m_findVisBegin - m_visibiBegin);   // tess-visual-thread
		findvis    += (m_lightingBegin - m_findVisBegin); // tess-visual-thread
		lighting   += (m_lightingEnd - m_lightingBegin);  // tess-visual-thread
		meshgen    += (m_meshGenEnd - m_meshBegin);       // tess-visual-thread
		waiting1   += (m_meshBegin - m_lightingEnd);      // tess-visual-thread
		waiting2   += (m_asyncEnd - m_asyncBegin);        // main-thread

		if (total > 1000000)
		{
			int frames = tickcnt - lasttickcnt;
			lasttickcnt = tickcnt;

			m_asyncString = StringUtil::Format("Cpu-time fps:%d total:%d main[%d-%d-%d..%d] thread[%d..%d-%d-%d..%d..%d]",
				frames,
				int(total / frames),
				int(preupdate / frames),
				int(preGfx / frames),
				int(lord / frames),
				int(waiting2 / frames),
				int(waitactive / frames),
				int(visibi / frames),
				int(findvis / frames),
				int(lighting / frames),
				int(meshgen / frames),
				int(waiting1 / frames));
			//LordLogInfo("%s", m_asyncString.c_str());
			ui64 unitt = 100000;
			String mainStr = StringUtil::Format("begin(%d) exch(%d) lord(%d) wait(%d-%d)",
				int(m_mainBegin%unitt),
				int(m_exchangeBegin%unitt),
				int(m_renderBegin%unitt),
				int(m_asyncBegin%unitt),
				int(m_asyncEnd%unitt));
			String tessStr = StringUtil::Format("active(%d) visi(%d) findvis(%d) lighting(%d) wait(%d-%d) meshgen(%d)",
				int(m_waitActive%unitt),
				int(m_visibiBegin%unitt),
				int(m_findVisBegin%unitt),
				int(m_lightingBegin%unitt),
				int(m_lightingEnd%unitt),
				int(m_meshBegin%unitt),
				int(m_meshGenEnd%unitt));
			//LordLogInfo("%s=====%s", mainStr.c_str(), tessStr.c_str());

			total = preupdate = preGfx = lord = visibi = findvis = lighting = meshgen = waiting1 = waiting2 = 0;
		}
	}

	
}

String Blockman::getRayTraceString()
{
	String result = "RayTrace ";
	if (objectMouseOver.result &&  objectMouseOver.typeOfHit == RAYTRACE_TYPE_BLOCK)
	{
		BlockPos pos = objectMouseOver.blockPos;
		int blockID = m_pWorld->getBlockId(pos);
		int meta = m_pWorld->getBlockMeta(pos);
		int skylight = m_pWorld->getSkyBlockTypeBrightness(LC_SKY, pos);
		int blockR = m_pWorld->getSkyBlockTypeBrightness(LC_BLOCK_R, pos);
		int blockG = m_pWorld->getSkyBlockTypeBrightness(LC_BLOCK_G, pos);
		int blockB = m_pWorld->getSkyBlockTypeBrightness(LC_BLOCK_B, pos);
		String name;
		if (blockID == 0)
			name = "air";
		else
			name = BlockManager::sBlocks[blockID]->getUnlocalizedName();
		result += StringUtil::Format("(%s)[%d,%d,%d] id:%d meta:%d  lit[%d,%d,%d,%d]", 
			name.c_str(), pos.x, pos.y, pos.z, blockID, meta, skylight, blockR, blockG, blockB);

		blockID = m_pWorld->getBlockId(pos.getPosY());
		meta = m_pWorld->getBlockMeta(pos.getPosY());
		skylight = m_pWorld->getSkyBlockTypeBrightness(LC_SKY, pos.getPosY());
		blockR = m_pWorld->getSkyBlockTypeBrightness(LC_BLOCK_R, pos.getPosY());
		blockG = m_pWorld->getSkyBlockTypeBrightness(LC_BLOCK_G, pos.getPosY());
		blockB = m_pWorld->getSkyBlockTypeBrightness(LC_BLOCK_B, pos.getPosY());
		result += StringUtil::Format("UpperBlock id:%d meta:%d lit[%d,%d,%d,%d]", blockID, meta, skylight, blockR, blockG, blockB);
	}

	return result;
}

void Blockman::runTick()
{
	PROFILE_CPU("Blockman::runTick");
	if (!m_pWorld->getWorldInfo().isTimeStopped())
	{
		m_pWorld->setWorldTime(m_pWorld->getWorldTime() + m_accelerateTime); // accelerate the time...
	}
														  // m_pWorld->tickBlocksAndAmbiance();
	m_entityRender->getMouseOver(1.0F);

	if (m_pWorld)
	{
		m_playerControl->updateController();
	}

	if (m_pPlayer)
	{
		if (m_pPlayer->getHealth() <= 0.0F)
		{
			// displayGuiScreen((GuiScreen)null);
		}
		else if (m_pPlayer->isPlayerSleeping() && m_pWorld)
		{
			// displayGuiScreen(new GuiSleepMP());
		}
	}

	if (m_pWorld)
	{
		// int buttonID;

		PROFILE_CPU("Blockman::runTick.handleInput");
		if (m_clickCounter > 0)
		{
			--m_clickCounter;
		}

		// change item.
		if (m_gameSettings->keyBindChange->pressed)
		{
			m_gameSettings->keyBindChange->pressed = false;
			m_pPlayer->inventory->changeCurrentItemToNext();
		}

		if (m_gameSettings->keyBindExit->pressed || m_gameSettings->keyBindAndroindBack->pressed)
		{
			exitPressed = true;
		}

		if (exitPressed && (!m_gameSettings->keyBindExit->pressed && !m_gameSettings->keyBindAndroindBack->pressed))
		{
			exitPressed = false;
			BackKeyDownEvent::emit();
		}

		if (m_pPlayer->isUsingItem())
		{
			if (m_gameSettings->userAction != GameSettings::ACTION_LONG_TOUCH_BEGIN && !m_pPlayer->m_isUsingItemByBtn)
			{
				m_playerControl->onStoppedUsingItem(m_pPlayer);
			}
		}
		else
		{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			static int f4keyColldown = 0;
			f4keyColldown++;
			if (GetAsyncKeyState(VK_F4) & 0x8000 && f4keyColldown > 10)
			{
				f4keyColldown = 0;
				m_rootGuiLayout->updateSwitchSerpece();
				int view = m_gameSettings->getPersonView() + 1;
				if (view > GameSettings::SPVT_SECOND)
					view = GameSettings::SPVT_FIRST;
				setPersonView(view);
			}

			// 锟斤拷前锟斤拷锟斤拷64锟阶匡拷锟街革拷锟
			static int f9keyColldown = 0;
			f9keyColldown++;
			if (GetAsyncKeyState(VK_F9) & 0x8000 && f9keyColldown > 10)
			{
				return;
				f9keyColldown = 0;
				
				Camera* mainCamera = SceneManager::Instance()->getMainCamera();
				Vector3 postion = mainCamera->getPosition();
				Vector3 direction = mainCamera->getDirection();
				direction.y = 0.f;
				direction.normalize();
				Vector3 tpPos = postion + direction * 64.f;
				Vector3i tpPosi(int(tpPos.x), 100, int(tpPos.z));
				if (m_pWorld->chunkExists(tpPosi.x >> 4, tpPosi.z >> 4))
				{
					ChunkPtr chunk = m_pWorld->getChunkFromBlockCoords(tpPosi.x, tpPosi.z);
					tpPosi.y = chunk->getHeightValue(tpPosi.x & 15, tpPosi.z & 15);
					String cmd = StringUtil::Format("/pangu tp %d %d %d", tpPosi.x, tpPosi.y, tpPosi.z);
					ClientNetwork::Instance()->getSender()->sendChatString(cmd.c_str());
				}
			}
#endif
			if (m_gameSettings->userAction == GameSettings::ACTION_CLICK) 
			{
				m_gameSettings->userAction = GameSettings::ACTION_NULL;

				if (m_clickCounter <= 0 || PlayerPrefs::Instance()->getBoolPrefs("banClickCD")) {
					clickScene();
					m_clickCounter = 5;
				}
				
			}
			else if (m_gameSettings->userAction == GameSettings::ACTION_LONG_TOUCH_BEGIN) {
				longTouchScene();
			}
			else if (m_gameSettings->userAction == GameSettings::ACTION_LONG_TOUCH_END) {
				m_gameSettings->userAction = GameSettings::ACTION_NULL;
			}

			if (m_gameSettings->userAction != GameSettings::ACTION_LONG_TOUCH_BEGIN)
			{
				m_lastUseItemId = -1;
			}
		}

		if (!m_pPlayer->isUsingItem() && m_cdDone) {
			sendClickSceneToController(m_gameSettings->userAction == GameSettings::ACTION_LONG_TOUCH_BEGIN);
		}
	}


	if (m_pWorld)
	{
		if (m_pPlayer)	
		{
			++joinPlayerCounter;

			if (joinPlayerCounter == 30)
			{
				joinPlayerCounter = 0;
				// m_pWorld->joinEntityInSurroundings(m_pPlayer);
			}
			renderMark();
		}

		m_entityRender->updateRenderer();
		m_globalRender->updateClouds();
		m_globalRender->renderDebugRaster();

		if (m_pWorld->m_lastLightningBolt > 0)
		{
			--m_pWorld->m_lastLightningBolt;
		}

		if (m_pPlayer->getHealth() > 0.0f)
		{
			m_pWorld->updateEntities();
			// m_pWorld->setAllowedSpawnTypes(m_pWorld->m_difficultySetting > 0, true);
			m_pWorld->tick();
			dynamic_cast<WorldClient*>(m_pWorld)->doVoidFogParticles(m_pPlayer->position.getFloor());
			EffectRenderer::Instance()->updateEffects();
		}
	}

	//m_mainControl->refresh();

	m_logicSystemTime = getSystemTime();
	if (m_hideGuiTime > 0 && m_logicSystemTime - m_hideGuiTime < 2000)
	{
		m_rootGuiLayout->hide();
	}
	else
	{
		if (m_rootGuiLayout)
		{
			m_rootGuiLayout->show();
		}
	}
}

void Blockman::setPersonView(int view)
{
	if ( !m_pPlayer || m_pPlayer->m_isCameraLocked)
		return;

	view = Math::Clamp(view, 0, 2);
	m_gameSettings->setPersonView(view);
	m_pPlayer->m_personView = view;
	m_pPlayer->m_changeToThirdPerspective = view == 1 ? true : false;
	refreshActorHide();
	m_rootGuiLayout->updateSwitchSerpece();
}

void Blockman::refreshActorHide()
{
	ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(m_pPlayer);
	if (actor) {
		if (m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST)
			actor->setActorHide(true);
		else
			actor->setActorHide(false);
	}
}

void Blockman::generateWorld(int sex, int defaultIdle)
{
	LoadWorldStartEvent::emit();

	Vector3i spawnPos = m_pWorld->getSpawnPoint();
	Camera* mainCamera = SceneManager::Instance()->getMainCamera();
	mainCamera->setPosition(Vector3(float(spawnPos.x), float(spawnPos.y + 6), float(spawnPos.z)));
	mainCamera->setTarget(Vector3(float(spawnPos.x - 5), float(spawnPos.y + 6), float(spawnPos.z)));
	mainCamera->setNearClip(0.1f);
	mainCamera->setFarClip(512.f);
	// mainCamera->setFov(Math::ATan(75*Math::DEG2RAD));

	int spawnChunkX = spawnPos.x >> 4;
	int spawnChunkZ = spawnPos.z >> 4;
	int loadedChunkCount = 0;
	int preloadRange = 3;
	i64 lastTime = LORD::Time::Instance()->getMillisecondsCPU();
	for (int offsetX = -preloadRange; offsetX <= preloadRange; ++offsetX)
	{
		for (int offsetZ = -preloadRange; offsetZ <= preloadRange; ++offsetZ)
		{
			i64 curTime = LORD::Time::Instance()->getMillisecondsCPU();

			if (curTime - lastTime > 1000L)
			{
				LordLogInfo("Preparing spawn area %d (%d/%d)", loadedChunkCount * 100 / 9, loadedChunkCount, 9);
				lastTime = curTime;
			}

			int x = spawnChunkX + offsetX;
			int z = spawnChunkZ + offsetZ;
			(void) m_pWorld->m_pChunkService->getChunk(x, z);
			++loadedChunkCount;
		}
	}
	loadWorld(m_pWorld, m_worldName, sex, defaultIdle);

	//SoundSystem::Instance()->setBackgroundMusicVolume(0.4f);
	//SoundSystem::Instance()->playBackgroundMusic("home", true); // config this bgm!
}

void Blockman::loadWorld(World* pWorld, const String& worldName, int sex, int defaultIdle)
{
	m_pWorld = pWorld;

	if (pWorld)
	{
		if (m_globalRender)
		{
			m_globalRender->setWorldAndLoadRenderers(pWorld);
		}

		auto dataCache = ClientNetwork::Instance()->getDataCache();
		if (m_pPlayer)
		{
			int oldServerId = dataCache->getServerId(m_pPlayer->entityId);
			dataCache->removeEntityByServerId(oldServerId, true);
			dataCache->removeClientServerIdPair(m_pPlayer->entityId);
			m_pPlayer = nullptr;
		}
		TeleportPositionConverter::ClearClientSideAnchors();
		m_pPlayer = m_playerControl->createClientPlayer(pWorld, m_interactionMgr);
		// there is no need to save the runtimeID in player.
		m_playerControl->flipPlayer(m_pPlayer);

		m_pPlayer->setPlatformUserId(GameClient::CGame::Instance()->getPlatformUserId());
		m_pPlayer->m_sex = sex;
		m_pPlayer->m_defaultIdle = defaultIdle;
		m_pPlayer->preparePlayerToSpawn();
		m_pPlayer->movementInput = LordNew MovementInputFromOptions(m_gameSettings);
		renderViewEntity = m_pPlayer;

		dataCache->addClientServerIdPair(m_pPlayer->entityId, dataCache->getSelfServerId());
		pWorld->spawnEntityInWorld(m_pPlayer);

		SCRIPT_EVENT::LuaLoadWorldEndEvent::invoke(true);

		LoadWorldEndEvent::emit();
	}
	else
	{
		m_pPlayer = NULL;
	}
}

void Blockman::onGameReady() 
{
	if (!m_canGameReady)
		return;

	if (!GameClient::CGame::Instance()->isDbDataReady())
		return;

	if (m_isGameReady)
		return;

	if (m_pPlayer)
		m_pPlayer->setPlatformUserId(GameClient::CGame::Instance()->getPlatformUserId());
	LordLogInfo("Blockman::onGameReady[Start]-------------------------->[%d]", Root::Instance()->getCurrentTime());
	m_isGameReady = true;
	m_rootGuiLayout->loadGameGui();

	SCRIPT_EVENT::GameReadyEvent::invoke(true);
	GameClient::CGame::Instance()->getNetwork()->getSender()->sendClientReady();
	GameClient::CGame::Instance()->getShellInterface()->loadMapComplete();
	LordLogInfo("Blockman::onGameReady[End]-------------------------->[%d]", Root::Instance()->getCurrentTime());
	SoundSystem::Instance()->stopBackgroundMusic();
}

void Blockman::rebirth(const Vector3& spawnPos, int sex, const String& showName, int defaultIdle, i32 vip)
{
	ui64 userId = 0;
	int teamId = 0;
	String teamName;
	float speedAddition{};
	String holdItemActor;
	vector<EntityFeature>::type entityFeatures;

	if (m_pPlayer)
	{
		userId = m_pPlayer->getPlatformUserId();
		teamId = m_pPlayer->getTeamId();
		teamName = m_pPlayer->getTeamName();
		speedAddition = m_pPlayer->getSpeedAddition();
		holdItemActor = m_pPlayer->m_holdItemActor;
		m_pWorld->removeEntity(m_pPlayer);
		m_pPlayer->setLogout(true);
		m_pPlayer->setDelayTicksToFree(5);

		for (auto entityFeature : m_pPlayer->m_entityFeatures)
		{
			EntityFeature feature;
			feature.m_name = entityFeature.m_name;
			feature.m_description = entityFeature.m_description;
			feature.m_duration = entityFeature.m_duration;;
			feature.m_scale = entityFeature.m_scale;;
			feature.m_type = entityFeature.m_type;;
			entityFeatures.push_back(feature);
		}

	}

	m_pPlayer = m_playerControl->createClientPlayer(m_pWorld, m_interactionMgr);
	m_pPlayer->setShowName(showName);
	m_pPlayer->m_sex = sex;
	m_pPlayer->m_vip = vip;
	m_pPlayer->m_defaultIdle = defaultIdle;
	m_pPlayer->setSpeedAddition(speedAddition);
	m_pPlayer->m_holdItemActor = holdItemActor;
	m_pPlayer->m_entityFeatures = entityFeatures;

	if (userId != 0)
	{
		m_pPlayer->setPlatformUserId(userId);
		m_pPlayer->setTeamId(teamId);
		m_pPlayer->setTeamName(teamName);
	}
	// there is no need to save the runtimeID in player.
	m_playerControl->flipPlayer(m_pPlayer);
	m_pPlayer->setPosition(spawnPos);

	m_pPlayer->preparePlayerToSpawn();
	m_pWorld->spawnEntityInWorld(m_pPlayer);
	m_pPlayer->movementInput = LordNew MovementInputFromOptions(m_gameSettings);
	renderViewEntity = m_pPlayer;
	SCRIPT_EVENT::PlayerRebirthEvent::invoke(m_pPlayer);
}

i32 Blockman::getSystemTime()
{
	return Root::Instance()->getCurrentTime();
}

void Blockman::sendClickSceneToController(bool touchHold)
{
	if (m_pPlayer->capabilities.isWatchMode || m_pPlayer->m_isCameraLocked)
	{
		return;
	}

	PROFILE_CPU("Blockman::sendClickSceneToController");
	if (!touchHold || !objectMouseOver.result)
	{
		if (m_playerControl)
		{
			m_playerControl->resetBlockRemoving();
			m_playerControl->onPlayerCancelLongHitEntity();
		}
		return;
	}

	if (!SCRIPT_EVENT::ClickSceneToControllerEvent::invoke(true))
	{
		return;
	}

	bool isPlayAction = true;
	bool isCanAttack = true;
	auto heldItem = m_pPlayer->getHeldItem();
	if (heldItem && heldItem->isGunItem())
	{
		isPlayAction = PlayerPrefs::Instance()->getBoolPrefs("IsGunAction");
		isCanAttack = PlayerPrefs::Instance()->getBoolPrefs("IsGunNorAttack");
	}
	
	if (objectMouseOver.typeOfHit == RAYTRACE_TYPE_BLOCK)
	{
		BlockPos blockPos = objectMouseOver.getBlockPos();
		if (isCanAttack)
		{
			m_playerControl->onPlayerDamageBlock(blockPos, objectMouseOver.sideHit->getIndex());
		}
		if (m_pPlayer->isCurrentToolAdventureModeExempt(blockPos) && isPlayAction)
		{
			// effectRenderer.addBlockHitEffects(blockPos.x, blockPos.y, blockPos.z, objectMouseOver->sideHit);
			m_pPlayer->swingItem();
		}
		return;
	}

	if (objectMouseOver.typeOfHit == RAYTRACE_TYPE_ENTITY)
	{
		Entity* pEntity = objectMouseOver.entityHit;
		if (pEntity->canLongHit() && pEntity->getLongHitTimes() > 0)
		{
			m_pPlayer->swingItem();
		}	
		return;
	}
}

void Blockman::longTouchScene() {
	if (m_useItemStatus == NOT_CAN_USE_ITEM)
	{
		return;
	}

	if (m_pPlayer->capabilities.isWatchMode || m_pPlayer->m_isCameraLocked)
	{
		return;
	}

	PROFILE_CPU("Blockman::longTouchScene");
	if (!SCRIPT_EVENT::LongTouchSceneEvent::invoke(true))
	{
		return;
	}

	ItemStackPtr pCurrent = m_pPlayer->inventory->getCurrentItem();

	if (pCurrent) 
	{
		if (longTouchUseType(pCurrent)) 
		{
			if (m_lastUseItemId != pCurrent->getItem()->itemID || canContinuousUse(pCurrent))
			{
				if (m_lastUseItemId == pCurrent->getItem()->itemID && 
					longtouchTick % pCurrent->getItem()->getUseDurationTick() != 0) 
				{
					longtouchTick++;
				}
				else 
				{
					m_playerControl->useItem(m_pPlayer, m_pWorld, pCurrent);
					longtouchTick = 1;
					m_lastUseItemId = pCurrent->getItem()->itemID;
				}
			}
		}
	}

	if (!m_pPlayer->isUsingItem()) {
		if (!objectMouseOver.result)
			return;

		if (objectMouseOver.typeOfHit == RAYTRACE_TYPE_BLOCK)
		{
			if (pCurrent && dynamic_cast<ItemToolGather*>(pCurrent->getItem()))
			{
				
				clickScene();
			}
			else
			{
				Vector3i blockPos = objectMouseOver.getBlockPos();
				int face = objectMouseOver.sideHit->getIndex();
				ClientNetwork::Instance()->getSender()->sendAnimate(m_pPlayer->entityId, NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_ATTACK);
				m_playerControl->clickBlock(blockPos, face);
			}
			return;
		}

		if (objectMouseOver.typeOfHit == RAYTRACE_TYPE_ENTITY)
		{
			m_playerControl->onPlayerLongHitEntity(objectMouseOver.entityHit);
			ClientNetwork::Instance()->getSender()->sendAnimate(m_pPlayer->entityId, NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_ATTACK);
			return;
		}
	}
}

void Blockman::clickScene()
{
	PROFILE_CPU("Blockman::clickScene");
	bool isPlayAction = true;
	bool isCanAttack = true;
	auto heldItem = m_pPlayer->getHeldItem();
	if (heldItem && heldItem->isGunItem())
	{
		isPlayAction = PlayerPrefs::Instance()->getBoolPrefs("IsGunAction");
		isCanAttack = PlayerPrefs::Instance()->getBoolPrefs("IsGunNorAttack");
	}

	m_cdDone = checkClickCoolingTime(heldItem);
	if (!m_cdDone)
	{
		return;
	}

	if (m_pPlayer->capabilities.isWatchMode || m_pPlayer->m_isCameraLocked)
	{
		return;
	}

	if (isPlayAction)
	{
		if (SCRIPT_EVENT::ClickScenePlayActionEvent::invoke(true))
			m_pPlayer->swingItem();
		else
			isPlayAction = false;
	}

	if (!SCRIPT_EVENT::ClickSceneEvent::invoke(true))
	{
		return;
	}

	AutoCastSkillEvent::emit();

	if (objectMouseOver.result == false)
	{
		m_pPlayer->playSound(ST_Air_Dig);
		return;
	}

	if (isPlayAction)
	{
		ClientNetwork::Instance()->getSender()->sendAnimate(m_pPlayer->entityId, NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_ATTACK);
	}

	if (objectMouseOver.typeOfHit == RAYTRACE_TYPE_ENTITY)
	{
		Entity* entityHit = objectMouseOver.entityHit;
		SCRIPT_EVENT::AttackEntityEvent::invoke(entityHit->entityId);

		int remoteEntityId = ClientNetwork::Instance()->getDataCache()->getServerId(entityHit->entityId);
		if (auto entityMerchant = dynamic_cast<EntityMerchant*>(entityHit))
		{
			if (!entityMerchant->getHasUpdated())
			{
				entityMerchant->setHasUpdated(true);
 				ClientNetwork::Instance()->getSender()->sendNeedUpdateMerchantCommodities(remoteEntityId);
			}
			MerchantActivateEvent::emit(entityMerchant);
		}
		else if (auto entityRank = dynamic_cast<EntityRankNpc*>(entityHit))
		{
			//RankActivateEvent::emit(entityRank->getRankInfo());
			ReceiveRankResultEvent::emit(GameClient::RankManager::Instance()->getRankInfo(entityRank->entityId));
			RankActivateEvent::emit(GameClient::RankManager::Instance()->getRankInfo(entityRank->entityId), entityRank->entityId);
		}
		else if (auto entityVehicle = dynamic_cast<EntityVehicle*>(entityHit))
		{
			m_pPlayer->askGetOnVehicle(entityVehicle);
		}
		else if (auto entityActorNpc = dynamic_cast<EntityActorNpc*>(entityHit))
		{
			m_pPlayer->attackActorNpc(entityActorNpc);
		}
		else if (auto entitySessionNpc = dynamic_cast<EntitySessionNpc*>(entityHit))
		{
			m_pPlayer->attackSessionNpc(entitySessionNpc);
		}
		else if (auto entityCreature = dynamic_cast<EntityCreature*>(entityHit))
		{
			m_pPlayer->attAckEntityCreature(entityCreature);
		} 
		else if (dynamic_cast<EntityBlockman*>(entityHit))
		{
			ClientNetwork::Instance()->getSender()->sendAttackEntity(entityHit->entityId);
		}
		else if (auto buildNpc = dynamic_cast<EntityBuildNpc*>(entityHit) )
		{
			if (buildNpc->m_userId == m_pPlayer->getPlatformUserId())
			{
				m_pPlayer->attackBuildNpc(buildNpc);
			}
		}
		else if (auto entityLandNpc= dynamic_cast<EntityLandNpc*>(entityHit))
		{
			if (entityLandNpc ->getUserId()  != m_pPlayer->getPlatformUserId())
			{
				return;
			}
			switch (entityLandNpc->getStatus()) {
			case 1:
				ShowRanchExpandTipEvent::emit(entityLandNpc->getLandCode(), entityLandNpc->getPrice(), entityLandNpc->getRecipe(), entityLandNpc->getReward());
				break;
			case 2:
				ShowRanchTimeTipEvent::emit(entityLandNpc->getLandCode(), entityLandNpc->getTotalTime(), entityLandNpc->getTimeLeft(), "gui_ranch_time_tip_extend");
				break;
			case 3:
				break;
			}
		}
		else if (auto entityActorCannon = dynamic_cast<EntityActorCannonClient*>(entityHit))
		{
		}
		else if (auto entityDecoration = dynamic_cast<EntityDecoration*>(entityHit))
		{
			ClientNetwork::Instance()->getSender()->sendAttackEntity(entityHit->entityId);
		}
		else
		{
			if (isCanAttack)
			{
				ClientNetwork::Instance()->getSender()->sendAttack(remoteEntityId, entityHit->position.x,
					entityHit->position.y, entityHit->position.z);
			}
		}
	}
	else if (objectMouseOver.typeOfHit == RAYTRACE_TYPE_BLOCK)
	{
		bool needToHandle = true;

		Vector3i blockPos = objectMouseOver.getBlockPos();
		int face = objectMouseOver.sideHit->getIndex();

		ItemStackPtr pStack = m_pPlayer->inventory->getCurrentItem();
		int size = (pStack ? pStack->stackSize : 0);
		float footY = m_pPlayer->position.y - (m_pPlayer->isSneaking() ? 2.54f : 2.62f);
		if (Math::Abs(footY - blockPos.y * 1.0f) <= 0.0001f && blockPos.y + 1 == objectMouseOver.hitPointPos.y && m_pPlayer->onGround
			&& m_pPlayer->position.x > blockPos.x - 0.3f && m_pPlayer->position.x < blockPos.x + 1.3f
			&& m_pPlayer->position.z > blockPos.z - 0.3f && m_pPlayer->position.z < blockPos.z + 1.3f)
		{
			int yaw = (int)m_pPlayer->rotationYaw % 360;
			yaw = yaw > 0 ? yaw : yaw + 360;
			if (yaw > 45 && yaw <= 135)
				face = BM_FACE_WEST;
			else if(yaw > 135 && yaw <= 225)
				face = BM_FACE_NORTH;
			else if (yaw > 225 && yaw <= 315)
				face = BM_FACE_EAST;
			else
				face = BM_FACE_SOUTH;
		}

		if (pStack && pStack->getItem() && pStack->getItem()->isRenderMark())
		{
			blockPos = m_newBolckPos;
		}

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
		{
			int yaw = (((int)m_pPlayer->rotationYaw) % 360 + 360 + 45) % 360 / 90 * 90;
			BlockPos playerPos = BlockPos((int)floor(m_pPlayer->position.x), ((int)(m_pPlayer->position.y - 1.6f)), (int)floor(m_pPlayer->position.z));
			switch (yaw)
			{
			case 90:
				playerPos = playerPos.getNegX2();
				break;
			case 180:
				playerPos = playerPos.getNegZ2();
				break;
			case 270:
				playerPos = playerPos.getPosX2();
				break;
			default:
				playerPos = playerPos.getPosZ2();
				break;
			}

			i32 blockId = m_pWorld->getBlockId(playerPos);
			if (blockId > 0 && dynamic_cast<BlockFruits*>(BlockManager::sBlocks[blockId]))
			{
				blockPos = playerPos;
			}
		}

		ItemStackPtr pCurrent = m_pPlayer->inventory->getCurrentItem();
		if (pCurrent && pCurrent->itemID != ITEM_ID_DEFENCE_TOWER && pCurrent->itemID != ITEM_ID_DEFENCE_WALL)
		{
			int placeBlockTimes = (m_pPlayer->m_canBuildBlockQuickly && m_pPlayer->m_quicklyBuildBlock) ? LogicSetting::Instance()->getQuicklyBuildBlockNum() : PlayerPrefs::Instance()->getIntPrefs("QuicklyBuildBlockNum");
			BlockPos currBlockPos = blockPos;
			BlockPos nextBlockPos = blockPos;

			for (int i = 1; i <= placeBlockTimes; ++i)
			{
				if (i != placeBlockTimes)
				{
					int blockID = m_pWorld->getBlockId(nextBlockPos);
					if (blockID == BLOCK_ID_SNOW)
					{
						face = BM_FACE_UP;
					}
					else if (blockID != BLOCK_ID_VINE && blockID != BLOCK_ID_TALL_GRASS && blockID != BLOCK_ID_DEADBUSH)
					{
						switch (face)
						{
						case BM_FACE_DOWN:	--nextBlockPos.y; break;
						case BM_FACE_UP:	++nextBlockPos.y; break;
						case BM_FACE_NORTH: --nextBlockPos.z; break;
						case BM_FACE_SOUTH: ++nextBlockPos.z; break;
						case BM_FACE_WEST:	--nextBlockPos.x; break;
						case BM_FACE_EAST:	++nextBlockPos.x; break;
						default: break;
						}
					}
				}

				if (!m_playerControl->onPlayerClickToBlock(m_pPlayer, m_pWorld, pStack, currBlockPos, face, objectMouseOver.hitPointPos))
				{
					break;
				}
				else if (i == 1)
				{
					needToHandle = false;
					if (isPlayAction)
						m_pPlayer->swingItem();
				}

				currBlockPos = nextBlockPos;
			}
		}
		else
		{
			if (m_playerControl->onPlayerClickToBlock(m_pPlayer, m_pWorld, pStack, blockPos, face, objectMouseOver.hitPointPos))
			{
				needToHandle = false;
				if (isPlayAction)
					m_pPlayer->swingItem();
			}
		}

		if (!pStack)
		{
			return;
		}

		if (pStack->stackSize == 0)
		{
			auto index = m_pPlayer->inventory->findItemStack(pStack);
			// remove ItemStack
			m_pPlayer->inventory->decrStackSize(index, 0);
		}
		
		if (needToHandle) {
			ItemStackPtr pCurrent = m_pPlayer->inventory->getCurrentItem();

			if (pCurrent) {
				if (pCurrent->itemID == Item::fishingRod->itemID)
				{
					return;
				}
				if (longTouchUseType(pCurrent))
				{
					return;
				}
				Item* pItem = Item::itemsList[pCurrent->itemID];
				if (!pItem)
				{
					return;
				}
				if (pItem->getItemUseAction(pCurrent) == ACTION_TYPE_BLOCK)
				{
					return;
				}
				m_playerControl->useItem(m_pPlayer, m_pWorld, pCurrent);
			}
		}
	}
}

bool Blockman::canContinuousUse(ItemStackPtr pStack)
{
	if (!pStack) {
		return false;
	}
	Item* item = pStack->getItem();
	return item && item->canContinuousUse();
}

bool Blockman::longTouchUseType(ItemStackPtr pStack)
{
	if (!pStack) {
		return false;
	}
	Item* item = pStack->getItem();
	return item && item->canLongTouchUse();
}

//void Blockman::setDimensionAndSpawnPlayer(int dimension)
//{
//	m_pWorld->setSpawnLocation();
//	// m_pWorld->removeAllEntities();
//
//	int entityID = 0;
//	// String var3;
//
//	if (m_pPlayer)
//	{
//		entityID = m_pPlayer->entityId;
//		m_pWorld->removeEntity(m_pPlayer);
//		// var3 = m_pPlayer.func_142021_k();
//		LordSafeDelete(m_pPlayer);
//	}
//
//	renderViewEntity = NULL;
//	m_pPlayer = m_playerControl->createClientPlayer(m_pWorld, m_interactionMgr);
//	m_pPlayer->dimension = dimension;
//	renderViewEntity = m_pPlayer;
//	m_pPlayer->preparePlayerToSpawn();
//	// m_pPlayer.func_142020_c(var3);
//	m_pWorld->spawnEntityInWorld(m_pPlayer);
//	m_playerControl->flipPlayer(m_pPlayer);
//	m_pPlayer->movementInput = LordNew MovementInputFromOptions(m_gameSettings);
//	m_pPlayer->entityId = entityID;
//}

//modify by maxicheng
void Blockman::saveChunks()
{
	throw std::logic_error("this method is not implemented");
}

//add by maxicheng

void Blockman::initializeNoGUI()
{
	BlockManager* bm = LordNew BlockManager();
	bm->initialize();
	ToolMaterial::initialize();
	ArmorMaterial::initialize();
	Potion::initialize();
	Item::initialize();
}

void Blockman::init(i64 seed)
{
	m_worldSettings = LordNew WorldSettings(seed, GAME_TYPE_SURVIVAL, false, false, TERRAIN_TYPE_DEFAULT);
	m_pWorld = LordNew WorldClient(m_worldName, *m_worldSettings, 64);
	m_pWorld->createChunkService(64);
}

void Blockman::switchPerspece() {
	int view = m_gameSettings->getPersonView() + 1;
	if (view > GameSettings::SPVT_SECOND)
		view = GameSettings::SPVT_FIRST;
	setPersonView(view);
}

int Blockman::getCurrPersonView() {
	return m_gameSettings->getPersonView();
}

void Blockman::setPerspece(int view) 
{
	setPersonView(view);
}

void Blockman::syncBlockDestroyeFailure(bool destroyeStatus, int destroyeFailureType)
{
	//TODO
}

void Blockman::setBloomThreshold(float threshold)
{
	auto postProcessStage = static_cast<PostProcessRenderStage*>(
		RenderStageManager::Instance()->getRenderStageByID(RSI_PostProcess));
	/*postProcessStage->setBloomThreshold(threshold);*/
}

void Blockman::setBloomSaturation(float saturation)
{
	auto postProcessStage = static_cast<PostProcessRenderStage*>(
		RenderStageManager::Instance()->getRenderStageByID(RSI_PostProcess));
	/*postProcessStage->setBloomSaturation(saturation);*/
}

void Blockman::setBloomIntensity(float intensity)
{
	auto postProcessStage = static_cast<PostProcessRenderStage*>(
		RenderStageManager::Instance()->getRenderStageByID(RSI_PostProcess));
	/*postProcessStage->setBloomIntensity(intensity);*/
}

void Blockman::setBloomBlurDeviation(float deviation)
{
	auto postProcessStage = static_cast<PostProcessRenderStage*>(
		RenderStageManager::Instance()->getRenderStageByID(RSI_PostProcess));
	/*postProcessStage->setBloomBlurDeviation(deviation);*/
}

void Blockman::setBloomBlurMultiplier(float multiplier)
{
	auto postProcessStage = static_cast<PostProcessRenderStage*>(
		RenderStageManager::Instance()->getRenderStageByID(RSI_PostProcess));
	/*postProcessStage->setBloomBlurMultiplier(multiplier);*/
}

void Blockman::setBlockBloomOption(float deviation, float intensity, float saturation)
{
	/*auto bloomBlockStage = static_cast<BloomBlockRenderStage*>(
		RenderStageManager::Instance()->getRenderStageByID(RSI_BlockBloomProcess));
	bloomBlockStage->setOption(deviation, intensity, saturation);*/
}

void Blockman::enableFullscreenBloom(bool enabled)
{
	auto postProcessStage = RenderStageManager::Instance()->getRenderStageByID(RSI_PostProcess);
	/*static_cast<PostProcessRenderStage*>(postProcessStage)->setEnableFullscreenBloom(enabled);*/
}

void Blockman::setBlockLightEnhancement(float enhancement)
{
	/*SceneManager::Instance()->setBlockLightEnhancement(enhancement);*/
}

Vector3 Blockman::getPlayerPosition()
{
	return m_pPlayer->getPosition(m_render_dt);
}

BlockPos Blockman::getBlockPosition()
{
	if (objectMouseOver.result &&  objectMouseOver.typeOfHit == RAYTRACE_TYPE_BLOCK)
	{
		return objectMouseOver.blockPos;
	}
	return BlockPos::ZERO;
}

float Blockman::getPlayerYaw()
{
	return m_pPlayer->rotationYaw;
}

int Blockman::getBlockId()
{
	BlockPos pos = getBlockPosition();
	if (pos != BlockPos::ZERO)
	{
		return m_pWorld->getBlockId(pos);
	}
	return 0;
}

void Blockman::beginTessVisual_stage1(RenderGlobal* rg, WorldClient* world)
{
	if (rg && m_visualTessThread)
		m_visualTessThread->activeStage1(rg, world);
}

void Blockman::beginTessVisual_stage2(RenderGlobal* rg, WorldClient* world)
{
	if (rg && m_visualTessThread)
		m_visualTessThread->activeStage2();
}

void Blockman::finishAsync()
{
	if(m_visualTessThread && m_visualTessThread->IsActived())
		m_visualTessThread->WaitForFinish(0xFFFFFFFF);
}

void Blockman::initCloseup(bool isShow, const Vector3& playerPos, const Vector3& closeupPos, float farDistance, float nearDistance, float velocity, float duration, float yaw, float pitch)
{
	m_closeup_isShow = isShow;
	m_closeup_playerPos = playerPos;
	m_closeup_closeupPos = closeupPos;
	m_closeup_farDis = farDistance;
	m_closeup_nearDis = nearDistance;
	m_closeup_velocity = velocity;
	m_closeup_duration = duration;
	m_closeup_yaw = yaw;
	m_closeup_pitch = pitch;
}

void Blockman::resetCloseup()
{
	m_playerControl->setDisableMovement(false);
	ClientNetwork::Instance()->getSender()->sendResetCloseup(m_closeup_playerPos);
	m_closeup_isShow = false;
	m_closeup_playerPos = Vector3::ZERO;
	m_closeup_closeupPos = Vector3::ZERO;
	m_closeup_farDis = 0.0f;
	m_closeup_nearDis = 0.0f;
	m_closeup_velocity = 0.0f;
	m_closeup_duration = 0.0f;
	m_closeup_yaw = 0.0f;
	m_closeup_pitch = 0.0f;
}

bool Blockman::checkClickCoolingTime(ItemStackPtr heldItem)
{
	if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
	{
		i32 curTime = getSystemTime();
		Item* pItem = heldItem ? heldItem->getItem() : NULL;
		i32 cdTime = GatherToolSetting::getToolCdTime(pItem ? pItem->itemID : 0);
		if (curTime - m_useToolGatherTime < cdTime)
		{
			return false;
		}
	}
	m_useToolGatherTime = getSystemTime();
	return true;
}

void Blockman::ManualAttack()
{
	clickScene();
}

bool Blockman::setAllowUseItem(int status)
{
	m_useItemStatus = status;
	return true;
}

void Blockman::renderMark()
{
	ItemStackPtr pStack = m_pPlayer->inventory->getCurrentItem();
	if (objectMouseOver.typeOfHit == RAYTRACE_TYPE_BLOCK)
	{
		if (pStack &&  dynamic_cast<ItemBuild*>(pStack->getItem()))
		{
			Vector3 playerPos = Vector3(floor(m_pPlayer->position.x) + 0.5f, ((int)(m_pPlayer->position.y - 2.6f)) * 1.0f, floor(m_pPlayer->position.z) + 0.5f);

			int yaw = (((int)m_pPlayer->rotationYaw) % 360 + 360 + 22) % 360 / 45 * 45;
			auto item = dynamic_cast<ItemBuild*>(pStack->getItem());
			if (yaw == 0)
			{
				playerPos.z = playerPos.z + 1.5f + (item->getLength() / 2);

			}
			else if (yaw == 45)
			{
				playerPos.z = playerPos.z + 1.5f + (item->getLength() / 2);
				playerPos.x = playerPos.x - 1.5f - (item->getLength() / 2);

			}
			else if (yaw == 90)
			{
				//playerPos.z = playerPos.z + 0.5f - (item->getLength() / 2);
				playerPos.x = playerPos.x - 1.5f - (item->getLength() / 2);

			}
			else if (yaw == 135)
			{
				playerPos.z = playerPos.z - 1.5f - item->getLength() / 2;
				playerPos.x = playerPos.x - 1.5f - (item->getLength() / 2);
			}
			else if (yaw == 180)
			{
				playerPos.z = playerPos.z - 1.5f - item->getLength() / 2;
			}
			else if (yaw == 225)
			{
				playerPos.z = playerPos.z - 1.5f - item->getLength() / 2;
				playerPos.x = playerPos.x + 1.5f + (item->getLength() / 2);
			}
			else if (yaw == 270)
			{
				//playerPos.z = playerPos.z + 1.5f - (item->getLength() / 2);
				playerPos.x = playerPos.x + 1.5f + (item->getLength() / 2);
			}
			else if (yaw == 315)
			{
				playerPos.z = playerPos.z + 1.5f + (item->getLength() / 2);
				playerPos.x = playerPos.x + 1.5f + (item->getLength() / 2);
			}
			m_newBolckPos = BlockPos((int)floor(playerPos.x), (int)playerPos.y, (int)floor(playerPos.z));
			objectMouseOver.blockPos = m_newBolckPos;

			Vector3 beginPos = Vector3(floor(playerPos.x - item->getLength() / 2), playerPos.y + 1.f, floor(playerPos.z - item->getWidth() / 2));
			Vector3 endPos = Vector3(floor(playerPos.x + item->getLength() / 2), playerPos.y + item->getHeight(), floor(playerPos.z + item->getWidth() / 2));
			Vector3 size = Vector3(item->getWidth(), 0.05f, item->getLength());
			Color color = ((WorldClient*)m_pWorld)->checkRangeBlockIsAir(beginPos, endPos) ? Color(1.0f, 1.0f, 1.0f, 0.2f) : Color(1.0f, 0.25f, 0.25f, 0.4f);
			m_globalRender->setMarkRenderArea(beginPos, size, color);
		}
		else if (pStack && dynamic_cast<ItemHouse*>(pStack->getItem()))
		{
			int yaw = (((int)m_pPlayer->rotationYaw) % 360 + 360 + 45) % 360 / 90 * 90;
			auto item = dynamic_cast<ItemHouse*>(pStack->getItem());
			Vector3 playerPos = Vector3(m_pPlayer->position.x, m_pPlayer->position.y - 2.6f, m_pPlayer->position.z);
			if (yaw == 0)
			{
				playerPos.z = playerPos.z + 1;
				playerPos.x = playerPos.x - item->getLength() / 2;
			}
			else if (yaw == 90)
			{
				playerPos.x = playerPos.x - 1 - item->getLength();
				playerPos.z = playerPos.z - item->getLength() / 2;
			}
			else if (yaw == 180)
			{
				playerPos.z = playerPos.z - 2 - item->getLength();
				playerPos.x = playerPos.x - item->getLength() / 2;
			}
			else if (yaw == 270)
			{
				playerPos.x = playerPos.x + 2;
				playerPos.z = playerPos.z - item->getLength() / 2;
			}
			m_newBolckPos = BlockPos((int)playerPos.x, (int)playerPos.y, (int)playerPos.z);
			objectMouseOver.blockPos = m_newBolckPos;
			Vector3 beginPos = Vector3((float)m_newBolckPos.x, m_newBolckPos.y + 1.f, (float)m_newBolckPos.z);
			Vector3 endPos = Vector3(m_newBolckPos.x + item->getLength(), m_newBolckPos.y + item->getHeight(), m_newBolckPos.z + item->getWidth());
			Vector3 size = Vector3(item->getWidth(), 0.05f, item->getLength());
			Color color = ((WorldClient*)m_pWorld)->checkRangeBlockIsAir(beginPos, endPos) ? Color(1.0f, 1.0f, 1.0f, 0.2f) : Color(1.0f, 0.25f, 0.25f, 0.4f);
			m_globalRender->setMarkRenderArea(beginPos, size, color);
		}
		else if (pStack && dynamic_cast<ItemTemplate*>(pStack->getItem()))
		{
			int yaw = (((int)m_pPlayer->rotationYaw) % 360 + 360 + 45) % 360 / 90 * 90;
			auto item = dynamic_cast<ItemTemplate*>(pStack->getItem());

			Vector3 playerPos = Vector3(floor(m_pPlayer->position.x), ((int)(m_pPlayer->position.y - 2.6f)) * 1.0f, floor(m_pPlayer->position.z) + 0.5f);

			if (yaw == 0)
			{
				playerPos.z = playerPos.z + 1;
				playerPos.x = playerPos.x - item->getLength() / 2;
			}
			else if (yaw == 90)
			{
				playerPos.x = playerPos.x - 1 - item->getLength();
				playerPos.z = playerPos.z - item->getLength() / 2;
			}
			else if (yaw == 180)
			{
				playerPos.z = playerPos.z - 2 - item->getLength();
				playerPos.x = playerPos.x - item->getLength() / 2;
			}
			else if (yaw == 270)
			{
				playerPos.x = playerPos.x + 2;
				playerPos.z = playerPos.z - item->getLength() / 2;
			}

			m_newBolckPos = BlockPos((int)playerPos.x, (int)playerPos.y, (int)playerPos.z);
			objectMouseOver.blockPos = m_newBolckPos;
			Vector3 beginPos = Vector3((float)m_newBolckPos.x, m_newBolckPos.y + 1.f, (float)m_newBolckPos.z);
			Vector3 endPos = Vector3(m_newBolckPos.x + item->getLength() - 1, m_newBolckPos.y + item->getHeight(), m_newBolckPos.z + item->getWidth() -1);
			Vector3 size = Vector3(item->getWidth(), item->getHeight(), item->getLength());

			bool isCanPlace = UICommon::checkItemPlaceInManor(beginPos, endPos);

			EntityArr entities = m_pWorld->getEntitiesWithinAABB(ENTITY_CLASS_ENTITY, Box(beginPos, endPos));
			Color color = (entities.empty() && isCanPlace) ? Color(1.0f, 1.0f, 1.0f, 0.2f) : Color(1.0f, 0.25f, 0.25f, 0.4f);

			m_globalRender->setMarkRenderArea(beginPos, size, color);
		}
		else if (pStack &&  dynamic_cast<ItemDecoration*>(pStack->getItem()))
		{
			Vector3 playerPos = Vector3(floor(m_pPlayer->position.x) + 0.5f, ((int)(m_pPlayer->position.y - 2.6f)) * 1.0f, floor(m_pPlayer->position.z) + 0.5f);
			int yaw = (((int)m_pPlayer->rotationYaw) % 360 + 360 + 45) % 360 / 90 * 90;
			auto item = dynamic_cast<ItemDecoration*>(pStack->getItem());
			if (yaw == 0)
			{
				playerPos.z = playerPos.z + 1.5f + item->getWidth() / 2;
			}
			else if (yaw == 90)
			{
				playerPos.x = playerPos.x - 1.5f - item->getWidth() / 2;
			}
			else if (yaw == 180)
			{
				playerPos.z = playerPos.z - 1.5f - item->getWidth() / 2;
			}
			else if (yaw == 270)
			{
				//playerPos.z = playerPos.z + 0.5f - (item->getLength() / 2);
				playerPos.x = playerPos.x + 1.5f + item->getWidth() / 2;
			}
			m_newBolckPos = BlockPos((int)floor(playerPos.x), (int)playerPos.y, (int)floor(playerPos.z));
			objectMouseOver.blockPos = m_newBolckPos;


			Vector3 size = Vector3(yaw % 180 == 0 ? item->getLength() : item->getWidth(), item->getHeight(), yaw % 180 == 0 ? item->getWidth() : item->getLength());
			Vector3 beginPos = Vector3(floor(  playerPos.x - (yaw % 180 == 0 ? item->getLength() : item->getWidth()) / 2), playerPos.y + 1.f, floor(playerPos.z - (yaw % 180 == 0 ? item->getWidth() : item->getLength()) / 2));
			Vector3 endPos = beginPos + size - 1.f;
			//Vector3 endPos = Vector3(floor(playerPos.x + item->getLength() / 2), playerPos.y + item->getHeight(), floor(playerPos.z + item->getWidth() / 2));
			bool isCanPlace = UICommon::checkItemPlaceInManor(beginPos, endPos);
			Color color = (((WorldClient*)m_pWorld)->checkRangeBlockIsAir(beginPos, endPos + 1.f) && isCanPlace) ? Color(1.0f, 1.0f, 1.0f, 0.4f) : Color(1.0f, 0.25f, 0.25f, 0.4f);
			m_globalRender->setMarkRenderArea(beginPos, size, color);

		}
		m_globalRender->enableMarkRender(pStack && pStack->getItem() && pStack->getItem()->isRenderMark());
	}
	else
	{
		m_globalRender->enableMarkRender(false);
	}
	
}

void Blockman::hideGui()
{
	m_hideGuiTime = getSystemTime();
}

void Blockman::startCameraRotation(float firstStageTime, float secondStageTime, float angleTotal, float angleDelta, float rotateRadius)
{
	m_isRotatingCamera = true;
	m_playerControl->setDisableMovement(true);
	setPersonView(1);
	m_pPlayer->rotationPitch = 0.0f;
	m_gameSettings->setMoveState((int)(MoveState::RunState));
	m_entityRender->setCameraRotationParam(firstStageTime, secondStageTime, angleTotal, angleDelta, rotateRadius);
}

void Blockman::stopCameraRotation()
{
	m_isRotatingCamera = false;
	m_playerControl->setDisableMovement(false);
	setPersonView(0);
	m_entityRender->resetCameraRotationParam();
}

}
